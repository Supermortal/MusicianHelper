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

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr))
        {
            IMFSinkWriter *pSinkWriter = NULL;
            DWORD stream;

            hr = ve->InitializeSinkWriter(&pSinkWriter, &stream, L"testOutput.mp4");
            if (SUCCEEDED(hr))
            {
                // Send frames to the sink writer.
                LONGLONG rtStart = 0;
                DWORD cbAudioData = 0;

                for (DWORD i = 0; i < vs.videoFrameCount; ++i)
                {
                    hr = ve->WriteFrame(pSinkWriter, stream, rtStart, (byte*)b.bmBits);
                    if (FAILED(hr))
                    {
                        break;
                    }
                    rtStart += vs.videoFrameDuration;
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