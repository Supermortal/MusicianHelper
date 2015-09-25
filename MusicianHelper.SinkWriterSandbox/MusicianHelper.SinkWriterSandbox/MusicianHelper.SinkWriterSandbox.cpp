/*https://msdn.microsoft.com/en-us/library/windows/desktop/ff819477(v=vs.85).aspx*/
/*http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html*/
/*http://www.ucancode.net/Visual_C_Source_Code/Load-DIB-Bitmap-File-Save-Bitmap-file-Convert-dib-to-bitmap-c-rotate-bitmap-file-vc-example.htm*/
/*https://msdn.microsoft.com/en-us/library/windows/desktop/ee663624(v=vs.85).aspx*/
/*https://msdn.microsoft.com/en-us/library/windows/desktop/aa372553(v=vs.85).aspx*/
/*https://msdn.microsoft.com/en-us/library/windows/desktop/dd757929(v=vs.85).aspx*/
/*https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/110456dc-b7fb-479c-817c-bad5028e4102/how-to-add-audio-data-to-a-video-file-created-by-a-sinkwriter-?forum=mediafoundationdevelopment*/

#include "stdafx.h"
#include "VideoEncoder.h"

#define _AFXDLL

#include <afx.h>
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>
#include <atlstr.h>
#include <stack>
#include <mfreadwrite.h>
#include <Mmreg.h>

#define WINVER _WIN32_WINNT_WIN7

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <stdio.h>
#include <mferror.h>

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

HRESULT CreateMediaSource(
    PCWSTR pszURL,
    IPropertyStore *pConfig,    // Optional, can be NULL
    IMFMediaSource **ppSource
    )
{
    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* pSource = NULL;

    // Create the source resolver.
    HRESULT hr = MFCreateSourceResolver(&pSourceResolver);

    // Use the source resolver to create the media source.
    if (SUCCEEDED(hr))
    {
        MF_OBJECT_TYPE ObjectType;

        //DbgLog(L"CreateObjectFromURL");

        hr = pSourceResolver->CreateObjectFromURL(
            pszURL,
            MF_RESOLUTION_MEDIASOURCE,  // Create a media source.
            pConfig,                    // Configuration properties.
            &ObjectType,                // Receives the object type. 
            &pSource
            );

        //DbgLog(L"CreateObjectFromURL - FINISHED");

    }

    if (SUCCEEDED(hr))
    {
        hr = pSource->QueryInterface(IID_PPV_ARGS(ppSource));
    }

    SafeRelease(&pSourceResolver);
    SafeRelease(&pSource);
    return hr;
}

HRESULT GetSourceDuration(IMFMediaSource *pSource, MFTIME *pDuration)
{
    *pDuration = 0;

    IMFPresentationDescriptor *pPD = NULL;

    HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
    if (SUCCEEDED(hr))
    {
        hr = pPD->GetUINT64(MF_PD_DURATION, (UINT64*)pDuration);
        pPD->Release();
    }
    return hr;
}

//-------------------------------------------------------------------
//
// Writes a block of data to a file
//
// hFile: Handle to the file.
// p: Pointer to the buffer to write.
// cb: Size of the buffer, in bytes.
//
//-------------------------------------------------------------------

HRESULT WriteToFile(HANDLE hFile, void* p, DWORD cb)
{
    DWORD cbWritten = 0;
    HRESULT hr = S_OK;

    BOOL bResult = WriteFile(hFile, p, cb, &cbWritten, NULL);
    if (!bResult)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }
    return hr;
}

//-------------------------------------------------------------------
// FixUpChunkSizes
//
// Writes the file-size information into the WAVE file header.
//
// WAVE files use the RIFF file format. Each RIFF chunk has a data
// size, and the RIFF header has a total file size.
//-------------------------------------------------------------------

HRESULT FixUpChunkSizes(
    HANDLE hFile,           // Output file.
    DWORD cbHeader,         // Size of the 'fmt ' chuck.
    DWORD cbAudioData       // Size of the 'data' chunk.
    )
{
    HRESULT hr = S_OK;

    LARGE_INTEGER ll;
    /*ll.QuadPart = cbHeader - sizeof(FOURCC);*/
    ll.QuadPart = cbHeader - sizeof(DWORD);

    if (0 == SetFilePointerEx(hFile, ll, NULL, FILE_BEGIN))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    // Write the data size.

    if (SUCCEEDED(hr))
    {
        hr = WriteToFile(hFile, &cbAudioData, sizeof(cbAudioData));
    }

    if (SUCCEEDED(hr))
    {
        // Write the file size.
        ll.QuadPart = sizeof(DWORD);

        if (0 == SetFilePointerEx(hFile, ll, NULL, FILE_BEGIN))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        DWORD cbRiffFileSize = cbHeader + cbAudioData - 8;

        // NOTE: The "size" field in the RIFF header does not include
        // the first 8 bytes of the file. (That is, the size of the
        // data that appears after the size field.)

        hr = WriteToFile(hFile, &cbRiffFileSize, sizeof(cbRiffFileSize));
    }

    return hr;
}

//-------------------------------------------------------------------
// WriteWaveData
//
// Decodes PCM audio data from the source file and writes it to
// the WAVE file.
//-------------------------------------------------------------------

HRESULT WriteWaveData(
    HANDLE hFile,               // Output file.
    IMFSourceReader *pReader,   // Source reader.
    DWORD cbMaxAudioData,       // Maximum amount of audio data (bytes).
    DWORD *pcbDataWritten       // Receives the amount of data written.
    )
{
    HRESULT hr = S_OK;
    DWORD cbAudioData = 0;
    DWORD cbBuffer = 0;
    BYTE *pAudioData = NULL;

    IMFSample *pSample = NULL;
    IMFMediaBuffer *pBuffer = NULL;

    // Get audio samples from the source reader.
    while (true)
    {
        DWORD dwFlags = 0;

        // Read the next sample.
        hr = pReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0, NULL, &dwFlags, NULL, &pSample);

        if (FAILED(hr)) { break; }

        if (dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
            printf("Type change - not supported by WAVE file format.\n");
            break;
        }
        if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            printf("End of input file.\n");
            break;
        }

        if (pSample == NULL)
        {
            printf("No sample\n");
            continue;
        }

        // Get a pointer to the audio data in the sample.

        hr = pSample->ConvertToContiguousBuffer(&pBuffer);

        if (FAILED(hr)) { break; }


        hr = pBuffer->Lock(&pAudioData, NULL, &cbBuffer);

        if (FAILED(hr)) { break; }


        // Make sure not to exceed the specified maximum size.
        if (cbMaxAudioData - cbAudioData < cbBuffer)
        {
            cbBuffer = cbMaxAudioData - cbAudioData;
        }

        // Write this data to the output file.
        hr = WriteToFile(hFile, pAudioData, cbBuffer);

        if (FAILED(hr)) { break; }

        // Unlock the buffer.
        hr = pBuffer->Unlock();
        pAudioData = NULL;

        if (FAILED(hr)) { break; }

        // Update running total of audio data.
        cbAudioData += cbBuffer;

        if (cbAudioData >= cbMaxAudioData)
        {
            break;
        }

        SafeRelease(&pSample);
        SafeRelease(&pBuffer);
    }

    if (SUCCEEDED(hr))
    {
        printf("Wrote %d bytes of audio data.\n", cbAudioData);

        *pcbDataWritten = cbAudioData;
    }

    if (pAudioData)
    {
        pBuffer->Unlock();
    }

    SafeRelease(&pBuffer);
    SafeRelease(&pSample);
    return hr;
}


//-------------------------------------------------------------------
// CalculateMaxAudioDataSize
//
// Calculates how much audio to write to the WAVE file, given the
// audio format and the maximum duration of the WAVE file.
//-------------------------------------------------------------------

DWORD CalculateMaxAudioDataSize(
    IMFMediaType *pAudioType,    // The PCM audio format.
    DWORD cbHeader,              // The size of the WAVE file header.
    DWORD msecAudioData          // Maximum duration, in milliseconds.
    )
{
    UINT32 cbBlockSize = 0;         // Audio frame size, in bytes.
    UINT32 cbBytesPerSecond = 0;    // Bytes per second.

    // Get the audio block size and number of bytes/second from the audio format.

    cbBlockSize = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_BLOCK_ALIGNMENT, 0);
    cbBytesPerSecond = MFGetAttributeUINT32(pAudioType, MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 0);

    // Calculate the maximum amount of audio data to write.
    // This value equals (duration in seconds x bytes/second), but cannot
    // exceed the maximum size of the data chunk in the WAVE file.

    // Size of the desired audio clip in bytes:
    DWORD cbAudioClipSize = (DWORD)MulDiv(cbBytesPerSecond, msecAudioData, 1000);

    // Largest possible size of the data chunk:
    DWORD cbMaxSize = MAXDWORD - cbHeader;

    // Maximum size altogether.
    cbAudioClipSize = min(cbAudioClipSize, cbMaxSize);

    // Round to the audio block size, so that we do not write a partial audio frame.
    cbAudioClipSize = (cbAudioClipSize / cbBlockSize) * cbBlockSize;

    return cbAudioClipSize;
}

//-------------------------------------------------------------------
// WriteWaveHeader
//
// Write the WAVE file header.
//
// Note: This function writes placeholder values for the file size
// and data size, as these values will need to be filled in later.
//-------------------------------------------------------------------

HRESULT WriteWaveHeader(
    HANDLE hFile,               // Output file.
    IMFMediaType *pMediaType,   // PCM audio format.
    DWORD *pcbWritten           // Receives the size of the header.
    )
{
    HRESULT hr = S_OK;
    UINT32 cbFormat = 0;

    WAVEFORMATEX *pWav = NULL;

    *pcbWritten = 0;

    // Convert the PCM audio format into a WAVEFORMATEX structure.
    hr = MFCreateWaveFormatExFromMFMediaType(pMediaType, &pWav, &cbFormat);

    // Write the 'RIFF' header and the start of the 'fmt ' chunk.
    if (SUCCEEDED(hr))
    {
        DWORD header[] = {
            // RIFF header
            FCC('RIFF'),
            0,
            FCC('WAVE'),
            // Start of 'fmt ' chunk
            FCC('fmt '),
            cbFormat
        };

        DWORD dataHeader[] = { FCC('data'), 0 };

        hr = WriteToFile(hFile, header, sizeof(header));

        // Write the WAVEFORMATEX structure.
        if (SUCCEEDED(hr))
        {
            hr = WriteToFile(hFile, pWav, cbFormat);
        }

        // Write the start of the 'data' chunk

        if (SUCCEEDED(hr))
        {
            hr = WriteToFile(hFile, dataHeader, sizeof(dataHeader));
        }

        if (SUCCEEDED(hr))
        {
            *pcbWritten = sizeof(header) + cbFormat + sizeof(dataHeader);
        }
    }


    CoTaskMemFree(pWav);
    return hr;
}

//-------------------------------------------------------------------
// ConfigureAudioStream
//
// Selects an audio stream from the source file, and configures the
// stream to deliver decoded PCM audio.
//-------------------------------------------------------------------

HRESULT ConfigureAudioStream(
    IMFSourceReader *pReader,   // Pointer to the source reader.
    IMFMediaType **ppPCMAudio   // Receives the audio format.
    )
{
    IMFMediaType *pUncompressedAudioType = NULL;
    IMFMediaType *pPartialType = NULL;

    // Select the first audio stream, and deselect all other streams.
    HRESULT hr = pReader->SetStreamSelection(
        (DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);

    if (SUCCEEDED(hr))
    {
        hr = pReader->SetStreamSelection(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    }

    // Create a partial media type that specifies uncompressed PCM audio.
    hr = MFCreateMediaType(&pPartialType);

    if (SUCCEEDED(hr))
    {
        hr = pPartialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    }

    if (SUCCEEDED(hr))
    {
        hr = pPartialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    }

    // Set this type on the source reader. The source reader will
    // load the necessary decoder.
    if (SUCCEEDED(hr))
    {
        hr = pReader->SetCurrentMediaType(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            NULL, pPartialType);
    }

    // Get the complete uncompressed format.
    if (SUCCEEDED(hr))
    {
        hr = pReader->GetCurrentMediaType(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            &pUncompressedAudioType);
    }

    // Ensure the stream is selected.
    if (SUCCEEDED(hr))
    {
        hr = pReader->SetStreamSelection(
            (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            TRUE);
    }

    // Return the PCM format to the caller.
    if (SUCCEEDED(hr))
    {
        *ppPCMAudio = pUncompressedAudioType;
        (*ppPCMAudio)->AddRef();
    }

    SafeRelease(&pUncompressedAudioType);
    SafeRelease(&pPartialType);
    return hr;
}

//-------------------------------------------------------------------
// WriteWaveFile
//
// Writes a WAVE file by getting audio data from the source reader.
//
//-------------------------------------------------------------------

HRESULT WriteWaveFile(
    IMFSourceReader *pReader,   // Pointer to the source reader.
    HANDLE hFile,               // Handle to the output file.
    LONG msecAudioData          // Maximum amount of audio data to write, in msec.
    )
{
    HRESULT hr = S_OK;

    DWORD cbHeader = 0;         // Size of the WAVE file header, in bytes.
    DWORD cbAudioData = 0;      // Total bytes of PCM audio data written to the file.
    DWORD cbMaxAudioData = 0;

    IMFMediaType *pAudioType = NULL;    // Represents the PCM audio format.

    // Configure the source reader to get uncompressed PCM audio from the source file.

    hr = ConfigureAudioStream(pReader, &pAudioType);

    // Write the WAVE file header.
    if (SUCCEEDED(hr))
    {
        hr = WriteWaveHeader(hFile, pAudioType, &cbHeader);
    }

    // Calculate the maximum amount of audio to decode, in bytes.
    if (SUCCEEDED(hr))
    {
        cbMaxAudioData = CalculateMaxAudioDataSize(pAudioType, cbHeader, msecAudioData);

        // Decode audio data to the file.
        hr = WriteWaveData(hFile, pReader, cbMaxAudioData, &cbAudioData);
    }

    // Fix up the RIFF headers with the correct sizes.
    if (SUCCEEDED(hr))
    {
        hr = FixUpChunkSizes(hFile, cbHeader, cbAudioData);
    }

    SafeRelease(&pAudioType);
    return hr;
}

int main()
{
    //UINT64 duration = 30;

    //VideoSettings vs;
    //vs.videoFps = 60;
    //vs.videoBitRate = 800000;
    //vs.videoFps = 30;
    ///*vs.videoWidth = b.bmWidth;
    //vs.videoHeight = b.bmHeight;*/
    //vs.videoEncodingFormat = MFVideoFormat_H264;
    //vs.videoInputFormat = MFVideoFormat_RGB32;
    //vs.videoPels = 0;

    //VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\test.bmp", L"output.mp4", 30, vs);

    //ve->Encode();

    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    /*if (argc != 3)
    {
    printf("arguments: input_file output_file.wav\n");
    return 1;
    }*/

    const WCHAR *wszSourceFile = L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Audio\\Dangerzone.mp3";
    const WCHAR *wszTargetFile = L"audioOutput.wav";

    HRESULT hr = S_OK;

    const LONG MAX_AUDIO_DURATION_MSEC = 5000; // 5 seconds

    IMFSourceReader *pReader = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    // Initialize the COM library.
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Initialize the Media Foundation platform.
    if (SUCCEEDED(hr))
    {
        hr = MFStartup(MF_VERSION);
    }

    MFTIME mft;
    if (SUCCEEDED(hr)) {
        IMFMediaSource *ms = NULL;

        CreateMediaSource(
            wszSourceFile,
            NULL,    // Optional, can be NULL
            &ms
            );
        
        hr = GetSourceDuration(ms, &mft);
    }

    // Create the source reader to read the input file.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateSourceReaderFromURL(wszSourceFile, NULL, &pReader);
        if (FAILED(hr))
        {
            printf("Error opening input file: %S\n", wszSourceFile, hr);
        }
    }

    // Open the output file for writing.
    if (SUCCEEDED(hr))
    {
        hFile = CreateFile(wszTargetFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
            CREATE_ALWAYS, 0, NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            printf("Cannot create output file: %S\n", wszTargetFile, hr);
        }
    }

    // Write the WAVE file.
    if (SUCCEEDED(hr))
    {
        hr = WriteWaveFile(pReader, hFile, mft);
    }

    if (FAILED(hr))
    {
        printf("Failed, hr = 0x%X\n", hr);
    }

    // Clean up.
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }

    SafeRelease(&pReader);
    MFShutdown();
    CoUninitialize();

    return SUCCEEDED(hr) ? 0 : 1;
}