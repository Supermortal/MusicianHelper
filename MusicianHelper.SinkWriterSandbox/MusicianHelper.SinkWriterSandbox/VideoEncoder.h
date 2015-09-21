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

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")

#pragma once
class VideoEncoder
{
public:
    VideoEncoder(LPCWSTR filePath);
    ~VideoEncoder();
    // This function converts the given bitmap to a DIB.
    // Returns true if the conversion took place,
    // false if the conversion either unneeded or unavailable
    bool ConvertToDIB(HBITMAP& hBitmap);
    // This function converts the given bitmap to a DFB.
    // Returns true if the conversion took place,
    // false if the conversion either unneeded or unavailable
    bool ConvertToDFB(HBITMAP& hBitmap);
    HBITMAP LoadImageFromFilePath(LPCWSTR filePath);
    template <class T> void SafeRelease(T **ppT);
    int GetDIBFromHandle(HBITMAP hBitmap, BITMAP *bitmap);
    UINT64 CalcVideoFrameDuration();
    UINT32 CalcVideoFrameCount();
private:
    LPCWSTR mFilePath;
    HBITMAP mHBitmap;
    UINT32 mVideoWidth = 0;
    UINT32 mVideoHeight = 0;
    UINT32 mVideoFps = 30;
    UINT64 mVideoFrameDuration = 30000000;
    UINT32 mVideoBitRate = 800000;
    GUID   mVideoEncodingFormat = MFVideoFormat_H264;
    GUID   mVideoInputFormat = MFVideoFormat_RGB32;
    UINT32 mVideoPels = 0;
    UINT32 mVideoFrameCount = 1800;
    GUID   mAudioEncodingFormat = MFAudioFormat_MP3;
    UINT32 mAudioChannels = 1;
    UINT32 mAudioAvgBytesPerSecond = 11000;
    UINT32 mAudioSamplesPerSecond = 320;
    UINT64 mDuration = 60;
};

struct VideoSettings {
    UINT32 videoWidth = 0;
    UINT32 videoHeight = 0;
    UINT32 videoFps = 30;
    UINT64 videoFrameDuration = 30000000;
    UINT32 videoBitRate = 800000;
    GUID   videoEncodingFormat = MFVideoFormat_H264;
    GUID   videoInputFormat = MFVideoFormat_RGB32;
    UINT32 videoPels = 0;
    UINT32 videoFrameCount = 1800;
};

struct AudioSettings {
    GUID   audioEncodingFormat = MFAudioFormat_MP3;
    UINT32 audioChannels = 1;
    UINT32 audioAvgBytesPerSecond = 11000;
    UINT32 audioSamplesPerSecond = 320;
};

