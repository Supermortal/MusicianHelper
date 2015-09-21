/*https://msdn.microsoft.com/en-us/library/windows/desktop/ff819477(v=vs.85).aspx*/
/*http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html*/
/*http://www.ucancode.net/Visual_C_Source_Code/Load-DIB-Bitmap-File-Save-Bitmap-file-Convert-dib-to-bitmap-c-rotate-bitmap-file-vc-example.htm*/

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

// Format constants
UINT32 VIDEO_WIDTH = 640;
UINT32 VIDEO_HEIGHT = 480;
const UINT32 VIDEO_FPS = 30;
const UINT64 VIDEO_FRAME_DURATION = 10 * 1000 * 1000 / VIDEO_FPS;
const UINT32 VIDEO_BIT_RATE = 800000;
const GUID   VIDEO_ENCODING_FORMAT = MFVideoFormat_H264;
const GUID   VIDEO_INPUT_FORMAT = MFVideoFormat_RGB32;
UINT32 VIDEO_PELS = VIDEO_WIDTH * VIDEO_HEIGHT;
const UINT32 VIDEO_FRAME_COUNT = 20 * VIDEO_FPS;

// Buffer to hold the video frame data.
//DWORD videoFrameBuffer[VIDEO_PELS];

HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex)
{
    *ppWriter = NULL;
    *pStreamIndex = NULL;

    IMFSinkWriter   *pSinkWriter = NULL;
    IMFMediaType    *pMediaTypeOut = NULL;
    IMFMediaType    *pMediaTypeIn = NULL;
    DWORD           streamIndex;

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
    HBITMAP h = (HBITMAP)LoadImage(NULL, L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Images\\11218847_1332157350140654_2658102053793722126_n.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    BITMAP stBitmap;
    bool success = ConvertToDIB(h);
    int errorCode = GetObject(h, sizeof(BITMAP), &stBitmap);

    VIDEO_WIDTH = stBitmap.bmWidth;
    VIDEO_HEIGHT = stBitmap.bmHeight;
    VIDEO_PELS = VIDEO_WIDTH * VIDEO_HEIGHT;

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
                    hr = WriteFrame(pSinkWriter, stream, rtStart, (byte*)stBitmap.bmBits);
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