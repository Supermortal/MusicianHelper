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

#pragma once
class VideoEncoder
{
public:
    VideoEncoder(LPCWSTR imageFilePath);
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
    void SetAudioSettings(AudioSettings as);
    HRESULT FixUpChunkSizes(
        HANDLE hFile,           // Output file.
        DWORD cbHeader,         // Size of the 'fmt ' chuck.
        DWORD cbAudioData       // Size of the 'data' chunk.
        );
    HRESULT WriteWaveData(
        HANDLE hFile,               // Output file.
        IMFSourceReader *pReader,   // Source reader.
        DWORD cbMaxAudioData,       // Maximum amount of audio data (bytes).
        DWORD *pcbDataWritten       // Receives the amount of data written.
        );
    HRESULT WriteToFile(HANDLE hFile, void* p, DWORD cb);
    DWORD CalculateMaxAudioDataSize(
        IMFMediaType *pAudioType,    // The PCM audio format.
        DWORD cbHeader,              // The size of the WAVE file header.
        DWORD msecAudioData          // Maximum duration, in milliseconds.
        );
    HRESULT WriteWaveHeader(
        HANDLE hFile,               // Output file.
        IMFMediaType *pMediaType,   // PCM audio format.
        DWORD *pcbWritten           // Receives the size of the header.
        );
    HRESULT ConfigureAudioStream(
        IMFSourceReader *pReader,   // Pointer to the source reader.
        IMFMediaType **ppPCMAudio   // Receives the audio format.
        );
    HRESULT WriteWaveFile(
        IMFSourceReader *pReader,   // Pointer to the source reader.
        HANDLE hFile,               // Handle to the output file.
        LONG msecAudioData          // Maximum amount of audio data to write, in msec.
        );
    HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex, DWORD blockSize, LPCWSTR videoOutputFilePath);
    HRESULT WriteFrame(
        IMFSinkWriter *pWriter,
        DWORD streamIndex,
        const LONGLONG& rtStart,        // Time stamp.
        byte* vfb,
        IMFSourceReader **pReader,
        DWORD cbMaxAudioData,       // Maximum amount of audio data (bytes).
        DWORD *pcbDataWritten       // Receives the amount of data written.
        );
    void SetDuration(UINT64 duration);
private:
    LPCWSTR mImageFilePath;
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

