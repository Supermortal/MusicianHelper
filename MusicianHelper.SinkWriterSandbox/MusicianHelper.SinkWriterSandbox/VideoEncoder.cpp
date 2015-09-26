#include "stdafx.h"

#define _AFXDLL

#include <afx.h>
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>
#include <atlstr.h>
#include <stack>

#define WINVER _WIN32_WINNT_WIN7

#include <stdio.h>

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")

#include "VideoEncoder.h"

template <int a, int b, int c, int d>
struct FOURCC
{
    static const unsigned int value = (((((d << 8) | c) << 8) | b) << 8) | a;
};

unsigned int id(FOURCC<'a', 'b', 'c', 'd'>::value);

VideoEncoder::VideoEncoder(LPCWSTR imageFilePath, LPCWSTR audioFilePath, LPCWSTR videoOutputPath, UINT64 duration, VideoSettings vs)
{
    mImageFilePath = imageFilePath;
    mVideoOutputPath = videoOutputPath;
    mAudioFilePath = audioFilePath;
    mDuration = duration;
    SetVideoSettings(vs);
}

VideoEncoder::~VideoEncoder()
{

}

bool VideoEncoder::ConvertToDFB(HBITMAP& hBitmap) {
    bool bConverted = false;
    BITMAP stBitmap;
    if (GetObject(hBitmap, sizeof(stBitmap), &stBitmap) && stBitmap.bmBits)
    {
        // that is a DIB. Now we attempt to create
        // a DFB with the same sizes, and with the pixel
        // format of the display (to omit conversions
        // every time we draw it).
        HDC hScreen = GetDC(NULL);
        if (hScreen)
        {
            HBITMAP hDfb =
                CreateCompatibleBitmap(hScreen,
                stBitmap.bmWidth, stBitmap.bmHeight);
            if (hDfb)
            {
                // now let's ensure what we've created is a DIB.
                if (GetObject(hDfb, sizeof(stBitmap),
                    &stBitmap) && !stBitmap.bmBits)
                {
                    // ok, we're lucky. Now we have
                    // to transfer the image to the DFB.
                    HDC hMemSrc = CreateCompatibleDC(NULL);
                    if (hMemSrc)
                    {
                        HGDIOBJ hOldSrc = SelectObject(hMemSrc, hBitmap);
                        if (hOldSrc)
                        {
                            HDC hMemDst = CreateCompatibleDC(NULL);
                            if (hMemDst)
                            {
                                HGDIOBJ hOldDst = SelectObject(hMemDst, hDfb);
                                if (hOldDst)
                                {
                                    // transfer the image using BitBlt
                                    // function. It will probably end in the
                                    // call to driver's DrvCopyBits function.
                                    if (BitBlt(hMemDst, 0, 0,
                                        stBitmap.bmWidth, stBitmap.bmHeight,
                                        hMemSrc, 0, 0, SRCCOPY))
                                        bConverted = true; // success

                                    VERIFY(SelectObject(hMemDst, hOldDst));
                                }
                                VERIFY(DeleteDC(hMemDst));
                            }
                            VERIFY(SelectObject(hMemSrc, hOldSrc));
                        }
                        VERIFY(DeleteDC(hMemSrc));
                    }
                }

                if (bConverted)
                {
                    VERIFY(DeleteObject(hBitmap)); // it's no longer needed
                    hBitmap = hDfb;
                }
                else
                    VERIFY(DeleteObject(hDfb));
            }
            ReleaseDC(NULL, hScreen);
        }
    }
    return bConverted;
}

bool VideoEncoder::ConvertToDIB(HBITMAP& hBitmap) {
    bool bConverted = false;
    BITMAP stBitmap;
    if (GetObject(hBitmap, sizeof(stBitmap),
        &stBitmap) && !stBitmap.bmBits)
    {
        // that is a DFB. Now we attempt to create
        // a DIB with the same sizes and pixel format.
        HDC hScreen = GetDC(NULL);
        if (hScreen)
        {
            union {
                BITMAPINFO stBitmapInfo;
                BYTE pReserveSpace[sizeof(BITMAPINFO)
                    + 0xFF * sizeof(RGBQUAD)];
            };
            ZeroMemory(pReserveSpace, sizeof(pReserveSpace));
            stBitmapInfo.bmiHeader.biSize = sizeof(stBitmapInfo.bmiHeader);
            stBitmapInfo.bmiHeader.biWidth = stBitmap.bmWidth;
            stBitmapInfo.bmiHeader.biHeight = stBitmap.bmHeight;
            stBitmapInfo.bmiHeader.biPlanes = 1;
            stBitmapInfo.bmiHeader.biBitCount = stBitmap.bmBitsPixel;
            stBitmapInfo.bmiHeader.biCompression = BI_RGB;

            if (stBitmap.bmBitsPixel <= 8)
            {
                stBitmapInfo.bmiHeader.biClrUsed =
                    1 << stBitmap.bmBitsPixel;
                // This image is paletted-managed.
                // Hence we have to synthesize its palette.
            }
            stBitmapInfo.bmiHeader.biClrImportant =
                stBitmapInfo.bmiHeader.biClrUsed;

            PVOID pBits;
            HBITMAP hDib = CreateDIBSection(hScreen,
                &stBitmapInfo, DIB_RGB_COLORS, &pBits, NULL, 0);

            if (hDib)
            {
                // ok, we're lucky. Now we have
                // to transfer the image to the DFB.
                HDC hMemSrc = CreateCompatibleDC(NULL);
                if (hMemSrc)
                {
                    HGDIOBJ hOldSrc = SelectObject(hMemSrc, hBitmap);
                    if (hOldSrc)
                    {
                        HDC hMemDst = CreateCompatibleDC(NULL);
                        if (hMemDst)
                        {
                            HGDIOBJ hOldDst = SelectObject(hMemDst, hDib);
                            if (hOldDst)
                            {
                                if (stBitmap.bmBitsPixel <= 8)
                                {
                                    // take the DFB's palette and set it to our DIB
                                    HPALETTE hPalette =
                                        (HPALETTE)GetCurrentObject(hMemSrc, OBJ_PAL);
                                    if (hPalette)
                                    {
                                        PALETTEENTRY pPaletteEntries[0x100];
                                        UINT nEntries = GetPaletteEntries(hPalette,
                                            0, stBitmapInfo.bmiHeader.biClrUsed,
                                            pPaletteEntries);
                                        if (nEntries)
                                        {
                                            ASSERT(nEntries <= 0x100);
                                            for (UINT nIndex = 0; nIndex < nEntries; nIndex++)
                                                pPaletteEntries[nEntries].peFlags = 0;
                                            VERIFY(SetDIBColorTable(hMemDst, 0,
                                                nEntries, (RGBQUAD*)pPaletteEntries) == nEntries);

                                        }
                                    }
                                }

                                // transfer the image using BitBlt function.
                                // It will probably end in the
                                // call to driver's DrvCopyBits function.
                                if (BitBlt(hMemDst, 0, 0, stBitmap.bmWidth,
                                    stBitmap.bmHeight, hMemSrc, 0, 0, SRCCOPY))
                                    bConverted = true; // success

                                VERIFY(SelectObject(hMemDst, hOldDst));
                            }
                            VERIFY(DeleteDC(hMemDst));
                        }
                        VERIFY(SelectObject(hMemSrc, hOldSrc));
                    }
                    VERIFY(DeleteDC(hMemSrc));
                }

                if (bConverted)
                {
                    VERIFY(DeleteObject(hBitmap)); // it's no longer needed
                    hBitmap = hDib;
                }
                else
                    VERIFY(DeleteObject(hDib));
            }
            ReleaseDC(NULL, hScreen);
        }
    }
    return bConverted;
}

template <class T> void VideoEncoder::SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

HBITMAP VideoEncoder::LoadImageFromFilePath(LPCWSTR filePath) {
    return (HBITMAP)LoadImage(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

int VideoEncoder::GetDIBFromHandle(HBITMAP hBitmap, BITMAP *bitmap) {
    bool success = ConvertToDIB(hBitmap);
    int errorCode = GetObject(hBitmap, sizeof(BITMAP), bitmap);
    return errorCode;
}

UINT64 VideoEncoder::CalcVideoFrameDuration(VideoSettings vs) {
    return 10 * 1000 * 1000 / vs.videoFps;
}

UINT32 VideoEncoder::CalcVideoFrameCount(VideoSettings vs, int duration) {
    return duration * vs.videoFps;
}

//UINT32 VideoEncoder::CalcVideoPels(UINT32 videoWidth, UINT32 videoHeight) {
//    return videoWidth * videoHeight;
//}

void VideoEncoder::SetVideoSettings(VideoSettings vs) {
    mVideoBitRate = vs.videoBitRate;
    mVideoEncodingFormat = vs.videoEncodingFormat;
    mVideoFps = vs.videoFps;
    //mVideoFrameCount = vs.videoFrameCount;
    //mVideoFrameDuration = vs.videoFrameDuration;
    //mVideoHeight = vs.videoHeight;
    mVideoInputFormat = vs.videoInputFormat;
    //mVideoPels = vs.videoPels;
    //mVideoWidth = vs.videoWidth;
}

void VideoEncoder::SetAudioSettings(AudioSettings as) {
    mAudioAvgBytesPerSecond = as.audioAvgBytesPerSecond;
    mAudioChannels = as.audioChannels;
    mAudioEncodingFormat = as.audioEncodingFormat;
    mAudioSamplesPerSecond = as.audioSamplesPerSecond;
}

HRESULT VideoEncoder::InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex, DWORD *pAudioStreamIndex, IMFMediaType **pAudioType, LPCWSTR videoOutputFilePath)
{
    *ppWriter = NULL;
    *pStreamIndex = NULL;

    IMFSinkWriter   *pSinkWriter = NULL;
    IMFMediaType    *pMediaTypeOut = NULL;
    IMFMediaType    *pMediaTypeOutAudio = NULL;
    IMFMediaType    *pMediaTypeIn = NULL;
    IMFMediaType    *pMediaTypeInAudio = NULL;
    DWORD           streamIndex;
    DWORD           audioStreamIndex;

    HRESULT hr = MFCreateSinkWriterFromURL(videoOutputFilePath, NULL, NULL, &pSinkWriter);

    // Set the output media type.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeOut);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, mVideoEncodingFormat);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, mVideoBitRate);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, mVideoWidth, mVideoHeight);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, mVideoFps, 1);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSinkWriter->AddStream(pMediaTypeOut, &streamIndex);
    }

    IMFSourceReader *pReader = NULL;
    if (SUCCEEDED(hr))
    {
        hr = MFCreateSourceReaderFromURL(mAudioFilePath, NULL, &pReader);
        if (FAILED(hr))
        {
            printf("Error opening input file: %S\n", mAudioFilePath, hr);
        }
    }

    //IMFMediaType *pAudioType = NULL;
    if (SUCCEEDED(hr)) {
        ConfigureAudioStream(pReader, &pMediaTypeInAudio);
    }

    GUID g;
    UINT32 bps;
    UINT32 nc;
    UINT32 sps;
    if (SUCCEEDED(hr))
    {
        pMediaTypeInAudio->GetGUID(MF_MT_SUBTYPE, &g);

        pMediaTypeInAudio->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &bps);

        pMediaTypeInAudio->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &nc);

        pMediaTypeInAudio->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sps);
    }

    // Set the output audio media type.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeOutAudio);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetGUID(MF_MT_SUBTYPE, g);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bps);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, nc);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, sps);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSinkWriter->AddStream(pMediaTypeOutAudio, &audioStreamIndex);
    }

    // Set the input media type.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeIn);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, mVideoInputFormat);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, mVideoWidth, mVideoHeight);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, mVideoFps, 1);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSinkWriter->SetInputMediaType(streamIndex, pMediaTypeIn, NULL);
    }

    // Set the input audio media type.
    //if (SUCCEEDED(hr))
    //{
    //    hr = MFCreateMediaType(&pMediaTypeInAudio);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = pMediaTypeInAudio->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = pMediaTypeInAudio->SetGUID(MF_MT_SUBTYPE, g);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bps);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, nc);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, sps);
    //}
    //if (SUCCEEDED(hr))
    //{
    //    hr = pSinkWriter->SetInputMediaType(audioStreamIndex, pAudioType, NULL);
    //}

    // Tell the sink writer to start accepting data.
    if (SUCCEEDED(hr))
    {
        hr = pSinkWriter->BeginWriting();
    }

    // Return the pointer to the caller.
    if (SUCCEEDED(hr))
    {
        *ppWriter = pSinkWriter;
        (*ppWriter)->AddRef();
        *pStreamIndex = streamIndex;
        *pAudioStreamIndex = audioStreamIndex;
        *pAudioType = pMediaTypeInAudio;
        (*pAudioType)->AddRef();
    }

    SafeRelease(&pSinkWriter);
    SafeRelease(&pMediaTypeOut);
    SafeRelease(&pMediaTypeOutAudio);
    SafeRelease(&pMediaTypeIn);
    SafeRelease(&pMediaTypeInAudio);
    SafeRelease(&pReader);
    return hr;
}

HRESULT VideoEncoder::WriteFrame(
    IMFSinkWriter *pWriter,
    DWORD streamIndex,
    const LONGLONG& rtStart,        // Time stamp.
    byte* vfb       // Receives the amount of data written.
    )
{
    IMFSample *pSample = NULL;
    IMFMediaBuffer *pBuffer = NULL;

    const LONG cbWidth = 4 * mVideoWidth;
    const DWORD cbBuffer = cbWidth * mVideoHeight;

    BYTE *pData = NULL;

    // Create a new memory buffer.
    HRESULT hr = MFCreateMemoryBuffer(cbBuffer, &pBuffer);

    // Lock the buffer and copy the video frame to the buffer.
    if (SUCCEEDED(hr))
    {
        hr = pBuffer->Lock(&pData, NULL, NULL);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFCopyImage(
            pData,                      // Destination buffer.
            cbWidth,                    // Destination stride.
            vfb,    // First row in source image.
            cbWidth,                    // Source stride.
            cbWidth,                    // Image width in bytes.
            mVideoHeight                // Image height in pixels.
            );
    }
    if (pBuffer)
    {
        pBuffer->Unlock();
    }

    // Set the data length of the buffer.
    if (SUCCEEDED(hr))
    {
        hr = pBuffer->SetCurrentLength(cbBuffer);
    }

    // Create a media sample and add the buffer to the sample.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateSample(&pSample);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSample->AddBuffer(pBuffer);
    }

    // Set the time stamp and the duration.
    if (SUCCEEDED(hr))
    {
        hr = pSample->SetSampleTime(rtStart);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSample->SetSampleDuration(mVideoFrameDuration);
    }

    // Send the sample to the Sink Writer.
    if (SUCCEEDED(hr))
    {
        hr = pWriter->WriteSample(streamIndex, pSample);
    }

    /*if (SUCCEEDED(hr)) {
        hr = WriteWaveData(pWriter, *pReader, cbMaxAudioData, pcbDataWritten);
        }*/

    SafeRelease(&pSample);
    SafeRelease(&pBuffer);
    return hr;
}

void VideoEncoder::SetDuration(UINT64 duration) {
    mDuration = duration;
}

HRESULT VideoEncoder::StartMediaFoundation() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr))
        {
            return hr;
        }
    }

    return ERROR;
}

UINT64 VideoEncoder::GetVideoFrameCount() {
    return mDuration * mVideoFps;
}

UINT64 VideoEncoder::GetVideoFrameDuration(){
    return 10 * 1000 * 1000 / mVideoFps;
}

void VideoEncoder::Encode() {
    HBITMAP h = LoadImageFromFilePath(mImageFilePath);

    BITMAP b;
    GetDIBFromHandle(h, &b);

    SetVideoHeightAndWidth(b);

    HRESULT hr = StartMediaFoundation();

    if (SUCCEEDED(hr)) {

        MFTIME mft;
        if (SUCCEEDED(hr)) {
            IMFMediaSource *ms = NULL;

            CreateMediaSource(
                mAudioFilePath,
                NULL,    // Optional, can be NULL
                &ms
                );

            hr = GetSourceDuration(ms, &mft);
            mDuration = mft / 10 / 1000 / 1000;
        }

        IMFSinkWriter *pSinkWriter = NULL;
        IMFMediaType *pAudioMediaType = NULL;
        DWORD stream;
        DWORD audioStream;

        UINT64 videoFrameDuration = GetVideoFrameDuration();
        UINT64 videoFrameCount = GetVideoFrameCount();

        hr = InitializeSinkWriter(&pSinkWriter, &stream, &audioStream, &pAudioMediaType, mVideoOutputPath);
        if (SUCCEEDED(hr))
        {
            // Send frames to the sink writer.
            LONGLONG rtStart = 0;
            DWORD cbAudioData = 0;

            for (DWORD i = 0; i < videoFrameCount; ++i)
            {
                hr = WriteFrame(pSinkWriter, stream, rtStart, (byte*)b.bmBits);
                if (FAILED(hr))
                {
                    break;
                }
                rtStart += videoFrameDuration;
            }
        }
        if (SUCCEEDED(hr))
        {
            DWORD max = CalculateMaxAudioDataSize(pAudioMediaType, mDuration * 1000);
            DWORD cbAudioData = 0;      // Total bytes of PCM audio data written to the file.
            IMFSourceReader *pReader = NULL;

            hr = MFCreateSourceReaderFromURL(mAudioFilePath, NULL, &pReader);

            hr = WriteWaveData(pSinkWriter, pReader, max, &cbAudioData, audioStream);

            char* t = "t";
        }
        if (SUCCEEDED(hr))
        {
            hr = pSinkWriter->Finalize();
        }
        SafeRelease(&pSinkWriter);
        MFShutdown();
    }

    CoUninitialize();
}

void VideoEncoder::SetVideoHeightAndWidth(BITMAP bitmap) {
    mVideoHeight = bitmap.bmHeight;
    mVideoWidth = bitmap.bmWidth;
}

HRESULT VideoEncoder::WriteWaveData(
    IMFSinkWriter *pWriter,
    //HANDLE hFile,               // Output file.
    IMFSourceReader *pReader,   // Source reader.
    DWORD cbMaxAudioData,       // Maximum amount of audio data (bytes).
    DWORD *pcbDataWritten,       // Receives the amount of data written.
    DWORD audioStreamIndex
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
        //hr = WriteToFile(hFile, pAudioData, cbBuffer);
        hr = pWriter->WriteSample(audioStreamIndex, pSample);

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

DWORD VideoEncoder::CalculateMaxAudioDataSize(
    IMFMediaType *pAudioType,    // The PCM audio format.
    DWORD msecAudioData          // Maximum duration, in milliseconds.
    )
{
    // The size of the WAVE file header.
    //might need to be changed
    DWORD cbHeader = 46;
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

HRESULT VideoEncoder::ConfigureAudioStream(
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

HRESULT VideoEncoder::CreateMediaSource(
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

HRESULT VideoEncoder::GetSourceDuration(IMFMediaSource *pSource, MFTIME *pDuration)
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
