/*https://msdn.microsoft.com/en-us/library/windows/desktop/ff819477(v=vs.85).aspx*/
/*http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html*/
/*http://www.ucancode.net/Visual_C_Source_Code/Load-DIB-Bitmap-File-Save-Bitmap-file-Convert-dib-to-bitmap-c-rotate-bitmap-file-vc-example.htm*/

#include "stdafx.h"

#define _AFXDLL

#include <afx.h>
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>
#include <atlstr.h>

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

// Format constants
const UINT32 VIDEO_WIDTH = 640;
const UINT32 VIDEO_HEIGHT = 480;
const UINT32 VIDEO_FPS = 30;
const UINT64 VIDEO_FRAME_DURATION = 10 * 1000 * 1000 / VIDEO_FPS;
const UINT32 VIDEO_BIT_RATE = 800000;
const GUID   VIDEO_ENCODING_FORMAT = MFVideoFormat_WMV3;
const GUID   VIDEO_INPUT_FORMAT = MFVideoFormat_RGB32;
const UINT32 VIDEO_PELS = VIDEO_WIDTH * VIDEO_HEIGHT;
const UINT32 VIDEO_FRAME_COUNT = 20 * VIDEO_FPS;

// Buffer to hold the video frame data.
DWORD videoFrameBuffer[VIDEO_PELS];

HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex)
{
    *ppWriter = NULL;
    *pStreamIndex = NULL;

    IMFSinkWriter   *pSinkWriter = NULL;
    IMFMediaType    *pMediaTypeOut = NULL;
    IMFMediaType    *pMediaTypeIn = NULL;
    DWORD           streamIndex;

    HRESULT hr = MFCreateSinkWriterFromURL(L"output.wmv", NULL, NULL, &pSinkWriter);

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
    return hr;
}

HRESULT WriteFrame(
    IMFSinkWriter *pWriter,
    DWORD streamIndex,
    const LONGLONG& rtStart,        // Time stamp.
    byte* vfb
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

    SafeRelease(&pSample);
    SafeRelease(&pBuffer);
    return hr;
}

void main()
{
    HBITMAP h = (HBITMAP)LoadImage(NULL, L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    bool b = (h == NULL);
    BITMAP bm;
    int hrr = GetObject(h, sizeof(BITMAP), &bm);

    b = ConvertToDIB(h);
    hrr = GetObject(h, sizeof(BITMAP), &bm);

    byte* arr2;
    hrr = GetObject(bm.bmBits, sizeof(byte), &arr2);

    ////b = ConvertToDFB(h);
    //b = ConvertToDIB(h);
    //hrr = GetObject(h, sizeof(BITMAP), &bm);

    int width = 2978;
    int height = 2240;
    long size = 20016694;

    byte* arr = LoadBMP(&width, &height, &size, L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp");

    // Set all pixels to green
    for (DWORD i = 0; i < VIDEO_PELS; ++i)
    {
        //videoFrameBuffer[i] = 0x0000FF00;
        videoFrameBuffer[i] = *(arr++);
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr))
        {
            IMFSinkWriter *pSinkWriter = NULL;
            DWORD stream;

            hr = InitializeSinkWriter(&pSinkWriter, &stream);
            if (SUCCEEDED(hr))
            {
                // Send frames to the sink writer.
                LONGLONG rtStart = 0;


                for (DWORD i = 0; i < VIDEO_FRAME_COUNT; ++i)
                {
                    hr = WriteFrame(pSinkWriter, stream, rtStart, arr);
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