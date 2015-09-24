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

VideoEncoder::VideoEncoder(LPCWSTR imageFilePath, LPCWSTR videoOutputPath, UINT64 duration, VideoSettings vs)
{
    mImageFilePath = imageFilePath;
    mVideoOutputPath = videoOutputPath;
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

UINT32 VideoEncoder::CalcVideoPels(UINT32 videoWidth, UINT32 videoHeight) {
    return videoWidth * videoHeight;
}

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

HRESULT VideoEncoder::InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex, LPCWSTR videoOutputFilePath)
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

    // Set the output audio media type.
    /*if (SUCCEEDED(hr))
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
    }*/

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
    /*if (SUCCEEDED(hr))
    {
    hr = MFCreateMediaType(&pMediaTypeInAudio);
    }
    if (SUCCEEDED(hr))
    {
    hr = pMediaTypeInAudio->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    }
    if (SUCCEEDED(hr))
    {
    hr = pMediaTypeInAudio->SetGUID(MF_MT_SUBTYPE, mAudioEncodingFormat);
    }
    if (SUCCEEDED(hr))
    {
    hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 320);
    }
    if (SUCCEEDED(hr))
    {
    hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, mAudioChannels);
    }
    if (SUCCEEDED(hr))
    {
    hr = pMediaTypeInAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, mAudioSamplesPerSecond);
    }
    if (SUCCEEDED(hr))
    {
    hr = pSinkWriter->SetInputMediaType(audioStreamIndex, pMediaTypeInAudio, NULL);
    }*/

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
    return 10 * 1000 * 1000 / mVideoFps;
}

UINT64 VideoEncoder::GetVideoFrameDuration(){
    return mDuration * mVideoFps;
}

void VideoEncoder::Encode() {
    HBITMAP h = LoadImageFromFilePath(mImageFilePath);

    BITMAP b;
    GetDIBFromHandle(h, &b);

    SetVideoHeightAndWidth(b);

    HRESULT hr = StartMediaFoundation();

    if (SUCCEEDED(hr)) {
        IMFSinkWriter *pSinkWriter = NULL;
        DWORD stream;

        UINT64 videoFrameDuration = GetVideoFrameDuration();
        UINT64 videoFrameCount = GetVideoFrameCount();

        hr = InitializeSinkWriter(&pSinkWriter, &stream, mVideoOutputPath);
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

UINT32 VideoEncoder::GetVideoPels() {
    return mVideoWidth * mVideoHeight;
}
