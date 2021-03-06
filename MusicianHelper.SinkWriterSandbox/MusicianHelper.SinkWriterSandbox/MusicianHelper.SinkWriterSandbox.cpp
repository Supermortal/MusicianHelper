/*https://msdn.microsoft.com/en-us/library/windows/desktop/ff819477(v=vs.85).aspx*/
/*http://tipsandtricks.runicsoft.com/Cpp/BitmapTutorial.html*/
/*http://www.ucancode.net/Visual_C_Source_Code/Load-DIB-Bitmap-File-Save-Bitmap-file-Convert-dib-to-bitmap-c-rotate-bitmap-file-vc-example.htm*/
/*https://msdn.microsoft.com/en-us/library/windows/desktop/ee663624(v=vs.85).aspx*/
/*https://msdn.microsoft.com/en-us/library/windows/desktop/aa372553(v=vs.85).aspx*/
/*https://msdn.microsoft.com/en-us/library/windows/desktop/dd757929(v=vs.85).aspx*/
/*https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/110456dc-b7fb-479c-817c-bad5028e4102/how-to-add-audio-data-to-a-video-file-created-by-a-sinkwriter-?forum=mediafoundationdevelopment*/
/*http://stackoverflow.com/questions/27352989/audio-sample-adding-to-a-video-using-sink-writer-in-windows-media-foundation*/
/*http://blogs.msdn.com/b/eternalcoding/archive/2013/03/06/developing-a-winrt-component-to-create-a-video-file-using-media-foundation.aspx*/
/*https://msdn.microsoft.com/en-us/library/windows/desktop/dd389281%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396*/
/*http://stackoverflow.com/questions/9022784/bitblt-drawing-bitmap-upside-down*/

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

#define WINVER _WIN32_WINNT_WIN7

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <stdio.h>
#include <mferror.h>
#include <string>

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

int main()
{
    HRESULT hr = S_OK;

    VideoSettings vs;
    vs.videoFps = 30;
    vs.videoBitRate = 800000;
    vs.videoEncodingFormat = MFVideoFormat_WMV3;

    VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp", L"C:\\Users\\user\\Dropbox\\Cloud\\GitHub\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav", L"output.wmv", vs);
    //VideoEncoder *ve = new VideoEncoder(L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Images\\paper-stained-3-texture.bmp", L"C:\\Users\\chpink\\Home\\sandbox\\MusicianHelper\\TEST\\Audio\\sorry_dave.wav", L"output.wmv", vs);

    ve->Encode();

    return SUCCEEDED(hr) ? 0 : 1;
}