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

struct VideoSettings {
    UINT32 videoFps = 30;
    UINT32 videoBitRate = 800000;
    GUID   videoEncodingFormat = MFVideoFormat_H264;
};

#pragma once
class VideoEncoder
{
public:
    VideoEncoder(LPCWSTR imageFilePath, LPCWSTR audioFilePath, LPCWSTR videoOutputPath, VideoSettings vs);
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
    UINT64 CalcVideoFrameDuration(VideoSettings vs);
    UINT32 CalcVideoFrameCount(VideoSettings vs, int duration);
    void SetVideoSettings(VideoSettings vs);
    HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex, DWORD *pAudioStreamIndex, LPCWSTR videoOutputPath, IMFSourceReader *pReader);
    HRESULT WriteFrame(
        IMFSinkWriter *pWriter,
        DWORD streamIndex,
        const LONGLONG& rtStart,        // Time stamp.
        byte* vfb
        );
    void SetDuration(UINT64 duration);
    HRESULT StartMediaFoundation();
    UINT64 GetVideoFrameCount();
    UINT64 GetVideoFrameDuration();
    void Encode();
    void SetVideoHeightAndWidth(BITMAP bitmap);
    HRESULT CreateMediaSource(
        PCWSTR pszURL,
        IPropertyStore *pConfig,    // Optional, can be NULL
        IMFMediaSource **ppSource
        );
    HRESULT GetSourceDuration(IMFMediaSource *pSource, MFTIME *pDuration);
    HRESULT ConfigureDecoder(IMFSourceReader *pReader, DWORD dwStreamIndex);
private:
    LPCWSTR mImageFilePath;
    LPCWSTR mVideoOutputPath;
    LPCWSTR mAudioFilePath;
    HBITMAP mHBitmap;
    UINT32 mVideoFps = 30;
    UINT64 mVideoFrameDuration = 30000000;
    UINT32 mVideoBitRate = 800000;
    GUID   mVideoEncodingFormat = MFVideoFormat_H264;
    GUID   mVideoInputFormat = MFVideoFormat_RGB32;
    UINT32 mVideoFrameCount = 1800;
    GUID   mAudioEncodingFormat = MFAudioFormat_MP3;
    UINT32 mAudioChannels = 1;
    UINT32 mAudioAvgBytesPerSecond = 176400;
    UINT32 mAudioSamplesPerSecond = 320;
    UINT64 mDuration = 5;
    UINT32 mVideoWidth = 0;
    UINT32 mVideoHeight = 0;
};

