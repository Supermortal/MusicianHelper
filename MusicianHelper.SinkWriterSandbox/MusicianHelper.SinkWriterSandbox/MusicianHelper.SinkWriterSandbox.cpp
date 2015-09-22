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
    UINT64 duration = 30;

    VideoSettings vs;
    vs.videoFps = 60;
    vs.videoBitRate = 800000;
    vs.videoFps = 30;
    /*vs.videoWidth = b.bmWidth;
    vs.videoHeight = b.bmHeight;*/
    vs.videoEncodingFormat = MFVideoFormat_H264;
    vs.videoInputFormat = MFVideoFormat_RGB32;
    vs.videoPels = 0;

    VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\test.bmp", L"output.mp4", 30, vs);

    ve->Encode();
}