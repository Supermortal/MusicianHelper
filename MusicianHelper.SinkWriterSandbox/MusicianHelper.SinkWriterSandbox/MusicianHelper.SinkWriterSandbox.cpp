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

HRESULT WriteWaveHeader(
    /*HANDLE hFile,   */            // Output file.
    IMFMediaType *pMediaType,   // PCM audio format.
    DWORD *pcbWritten
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

        /*hr = WriteToFile(hFile, header, sizeof(header));*/

        // Write the WAVEFORMATEX structure.
        //if (SUCCEEDED(hr))
        //{
        //    hr = WriteToFile(hFile, pWav, cbFormat);
        //}

        //// Write the start of the 'data' chunk

        //if (SUCCEEDED(hr))
        //{
        //    hr = WriteToFile(hFile, dataHeader, sizeof(dataHeader));
        //}

        if (SUCCEEDED(hr))
        {
            *pcbWritten = sizeof(header) + cbFormat + sizeof(dataHeader);
        }
    }


    //CoTaskMemFree(pWav);
    return hr;
}


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

BYTE* ConvertBMPToRGBBuffer(BYTE* Buffer, int width, int height)
{
    if ((NULL == Buffer) || (width == 0) || (height == 0))
        return NULL;
    /*Now we have to find out the number of bytes every scanline is padded with*/
        int padding = 0;
    int scanlinebytes = width * 3;
    while ((scanlinebytes + padding) % 4 != 0)
        padding++;
    /*At the end of the while loop padding will hold the number of padding bytes.

        Now we can get the length in bytes of a padded scanline :*/
    int psw = scanlinebytes + padding;
    /*And construct the buffer to hold the output*/
        BYTE* newbuf = new BYTE[width*height * 3];
    /*The 3 stands for the number of bytes in one RGBTriplet of course.*/

        /*Now comes the heart of the function :*/
    long bufpos = 0;
    long newpos = 0;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < 3 * width; x += 3)
        {
            newpos = y * 3 * width + x;
            bufpos = (height - y - 1) * psw + x;

            newbuf[newpos] = Buffer[bufpos + 2];
            newbuf[newpos + 1] = Buffer[bufpos + 1];
            newbuf[newpos + 2] = Buffer[bufpos];
        }
    /*What exactly happens in this loop ?
        For clear code and some more speed we declare two variables that will hold the buffer indices.
        The first for loop loops trough each scanline in the image data, the second loop hits every 3rd byte in a scanline, meaning the start of every RGB triplet(representing a pixel).
        Then we compute the index the current pixel will have in the new RGB buffer as current scanline * imagewidth * numberofbytesperpixel + position of current pixel.
        Next we compute the position we have to look at for the current pixel in the image data.The image was stored upside down in the.bmp, thus if we want to find a pixel color in the first line we have to look at the last scanline in the image data.Because we start indexing arrays with 0, the scanline to look for is imageheight - currentscanline(the y variable of the loop) - 1.
        To get the exact pixel position, we have to multiply the scanline number by the amount of bytes per scanline in the buffer, which we already computed in psw.And finally we add the x position of the current pixel.
        So now we have the position the pixel(x, y) will have in the new buffer in newpos, and the position the color values for this pixel are at in the image data is in bufpos.
        Now we could just assign those values, but remember that the color values themselves are stored in BGR format in the image, and we want them in RGB format, so we have to swap the bytes at our position(red value) and the one at our poition + 2 (blue value).
        I hope that was halfway clear : )

        Now we can finish the function :*/
    return newbuf;
}

BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFOHEADER bmpinfo;
    DWORD bytesread;
    /*Note that we take three pointers as parameters for width, height and size, since we will return the image dimensions and size in these variables.bmpfile is of course the filename of the bitmap, and the return value of the function will be a pointer to the image data.
        First lets try to open the file :*/
    HANDLE file = CreateFile(bmpfile, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (NULL == file)
        return NULL;
    /*Just a quick note here : it's useful to write if ( NULL == file ) instead of if ( file == NULL ) to prevent bugs, since on accidently typing if ( file = NULL ) the compiler will not complain but assign NULL to the file handle. if ( NULL = file ) will spawn a compiler error, so you can prevent bugs easily this way.
        Back to the topic : now we opened the file and can read the file header.On error we will close the file and return from the function.*/
    if (ReadFile(file, &bmpheader, sizeof(BITMAPFILEHEADER),
        &bytesread, NULL) == false)
    {
        CloseHandle(file);
        return NULL;
    }
    /*Now we can read the info header :*/
    if (ReadFile(file, &bmpinfo, sizeof(BITMAPINFOHEADER),
        &bytesread, NULL) == false)
    {
        CloseHandle(file);
        return NULL;
    }
    /*Since we are only going to load 24bit.bmps here we now do some checking of the header contents.
        First check if the file is actually a bitmap :*/
    if (bmpheader.bfType != 'MB')
    {
        CloseHandle(file);
        return NULL;
    }
    /*check if it's uncompressed*/
    if (bmpinfo.biCompression != BI_RGB)
    {
        CloseHandle(file);
        return NULL;
    }
    /*and check if it's 24bit*/
    if (bmpinfo.biBitCount != 24)
    {
        CloseHandle(file);
        return NULL;
    }
    /*When we are here we actually have a 24 bit bmp, so lets get its size and dimensions.We'll store them in the supplied variables:*/
    *width = bmpinfo.biWidth;
    *height = abs(bmpinfo.biHeight);
    *size = bmpheader.bfSize - bmpheader.bfOffBits;
    /*To be independent of the type of info header, we compute the imaga data size as the whole file size minus the distance from file origin to start of image data.
        Now we create a buffer to hold the data*/
    BYTE* Buffer = new BYTE[*size];
    /*Again, to be independent of info header version, we set the file pointer to the start of image data as told by the bfOffBits :*/
    SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);
    /*And now we can read in the data.We make sure that on error the Buffer gets deleted so we don't create memory leaks:*/
    if (ReadFile(file, Buffer, *size, &bytesread, NULL) == false)
    {
        delete[] Buffer;
        CloseHandle(file);
        return NULL;
    }
    /*and finish the function*/
    CloseHandle(file);
    return Buffer;
}

// This function converts the given bitmap to a DFB.
// Returns true if the conversion took place,
// false if the conversion either unneeded or unavailable
bool ConvertToDFB(HBITMAP& hBitmap)
{
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

// This function converts the given bitmap to a DIB.
// Returns true if the conversion took place,
// false if the conversion either unneeded or unavailable
bool ConvertToDIB(HBITMAP& hBitmap)
{
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

const UINT32 DURATION = 5;

// Format constants
UINT32 VIDEO_WIDTH = 0;
UINT32 VIDEO_HEIGHT = 0;
const UINT32 VIDEO_FPS = 30;
const UINT64 VIDEO_FRAME_DURATION = 10 * 1000 * 1000 / VIDEO_FPS;
const UINT32 VIDEO_BIT_RATE = 800000;
const GUID   VIDEO_ENCODING_FORMAT = MFVideoFormat_H264;
const GUID   VIDEO_INPUT_FORMAT = MFVideoFormat_RGB32;
UINT32 VIDEO_PELS = 0;
const UINT32 VIDEO_FRAME_COUNT = DURATION * VIDEO_FPS;

GUID   AUDIO_ENCODING_FORMAT = MFAudioFormat_MP3;
UINT32 AUDIO_CHANNELS = 1;
UINT32 AUDIO_AVG_BYTES_PER_SECOND = 11000;
UINT32 AUDIO_SAMPLES_PER_SECOND = 320;

// Buffer to hold the video frame data.
//DWORD videoFrameBuffer[VIDEO_PELS];

HRESULT WriteWaveData(
    /*HANDLE hFile, */              // Output file.
    IMFSinkWriter *ppWriter,
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
        //hr = WriteToFile(hFile, pAudioData, cbBuffer);
        ppWriter->WriteSample(1, pSample);

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

    HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex, DWORD blockSize)
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

    HRESULT hr = MFCreateSinkWriterFromURL(L"output.mp4", NULL, NULL, &pSinkWriter);

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
        hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, VIDEO_ENCODING_FORMAT);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, VIDEO_BIT_RATE);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, VIDEO_FPS, 1);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSinkWriter->AddStream(pMediaTypeOut, &streamIndex);
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
        hr = pMediaTypeOutAudio->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_MP3);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 320);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
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
        hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, VIDEO_INPUT_FORMAT);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, VIDEO_FPS, 1);
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
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeInAudio);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeInAudio->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeInAudio->SetGUID(MF_MT_SUBTYPE, AUDIO_ENCODING_FORMAT);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 320);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, AUDIO_CHANNELS);
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, AUDIO_SAMPLES_PER_SECOND);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSinkWriter->SetInputMediaType(audioStreamIndex, pMediaTypeInAudio, NULL);
    }

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
    }

    SafeRelease(&pSinkWriter);
    SafeRelease(&pMediaTypeOut);
    SafeRelease(&pMediaTypeIn);
    SafeRelease(&pMediaTypeInAudio);
    return hr;
}

HRESULT WriteFrame(
    IMFSinkWriter *pWriter,
    DWORD streamIndex,
    const LONGLONG& rtStart,        // Time stamp.
    byte* vfb,
    IMFSourceReader **pReader,
    DWORD cbMaxAudioData,       // Maximum amount of audio data (bytes).
    DWORD *pcbDataWritten       // Receives the amount of data written.
    )
{
    IMFSample *pSample = NULL;
    IMFMediaBuffer *pBuffer = NULL;

    const LONG cbWidth = 4 * VIDEO_WIDTH;
    const DWORD cbBuffer = cbWidth * VIDEO_HEIGHT;

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
            VIDEO_HEIGHT                // Image height in pixels.
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
        hr = pSample->SetSampleDuration(VIDEO_FRAME_DURATION);
    }

    // Send the sample to the Sink Writer.
    if (SUCCEEDED(hr))
    {
        hr = pWriter->WriteSample(streamIndex, pSample);
    }

    if (SUCCEEDED(hr)) {
        hr = WriteWaveData(pWriter, *pReader, cbMaxAudioData, pcbDataWritten);
    }

    SafeRelease(&pSample);
    SafeRelease(&pBuffer);
    return hr;
}

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

void main()
{
    VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\test.bmp");

    HBITMAP h = ve->LoadImageFromFilePath(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\test.bmp");

    BITMAP b;
    ve->GetDIBFromHandle(h, &b);

    UINT64 duration = 30;

    VideoSettings vs;
    vs.videoFps = 60;
    vs.videoBitRate = 800000;
    vs.videoFps = 30;
    vs.videoWidth = b.bmWidth;
    vs.videoHeight = b.bmHeight;
    vs.videoEncodingFormat = MFVideoFormat_H264;
    vs.videoInputFormat = MFVideoFormat_RGB32;
    vs.videoPels = 0;
    vs.videoFrameDuration = ve->CalcVideoFrameDuration(vs);
    vs.videoFrameCount = ve->CalcVideoFrameCount(vs, duration);
    
    ve->SetVideoSettings(vs);
    ve->SetDuration(duration);

    /*HBITMAP h = (HBITMAP)LoadImage(NULL, L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    BITMAP stBitmap;
    bool success = ConvertToDIB(h);
    int errorCode = GetObject(h, sizeof(BITMAP), &stBitmap);

    VIDEO_WIDTH = stBitmap.bmWidth;
    VIDEO_HEIGHT = stBitmap.bmHeight;
    VIDEO_PELS = VIDEO_WIDTH * VIDEO_HEIGHT;*/

    IMFSourceReader *pReader = NULL;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr))
        {
            hr = MFCreateSourceReaderFromURL(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Audio\\dangerzone.mp3", NULL, &pReader);
            if (FAILED(hr))
            {
                printf("Error opening input file: %S\n", L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav", hr);
            }

            IMFMediaType *pAudioType = NULL;
            /*hr = ConfigureAudioStream(pReader, &pAudioType);*/
            hr = pReader->GetCurrentMediaType(0, &pAudioType);

            GUID majorType;
            pAudioType->GetMajorType(&majorType);

            GUID subType;
            pAudioType->GetGUID(MF_MT_SUBTYPE, &subType);
            AUDIO_ENCODING_FORMAT = subType;

            DWORD pcbWritten;
            WriteWaveHeader(pAudioType, &pcbWritten);

            WAVEFORMATEX *pWav = NULL;
            UINT32 cbFormat = 0;
            // Convert the PCM audio format into a WAVEFORMATEX structure.
            hr = MFCreateWaveFormatExFromMFMediaType(pAudioType, &pWav, &cbFormat);

            AUDIO_AVG_BYTES_PER_SECOND = pWav->nAvgBytesPerSec;
            AUDIO_CHANNELS = pWav->nChannels;
            AUDIO_SAMPLES_PER_SECOND = pWav->nSamplesPerSec;

            DWORD blockSize = CalculateMaxAudioDataSize(pAudioType, pcbWritten, DURATION * 1000);

            IMFSinkWriter *pSinkWriter = NULL;
            DWORD stream;

            hr = ve->InitializeSinkWriter(&pSinkWriter, &stream, blockSize, L"testOutput.mp4");
            if (SUCCEEDED(hr))
            {
                // Send frames to the sink writer.
                LONGLONG rtStart = 0;
                DWORD cbAudioData = 0;

                for (DWORD i = 0; i < VIDEO_FRAME_COUNT; ++i)
                {
                    hr = ve->WriteFrame(pSinkWriter, stream, rtStart, (byte*)b.bmBits, &pReader, blockSize, &cbAudioData);
                    if (FAILED(hr))
                    {
                        break;
                    }
                    rtStart += VIDEO_FRAME_DURATION;
                }
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
}