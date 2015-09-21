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
    VideoEncoder();
    ~VideoEncoder();
    // This function converts the given bitmap to a DIB.
    // Returns true if the conversion took place,
    // false if the conversion either unneeded or unavailable
    bool ConvertToDIB(HBITMAP& hBitmap);
    // This function converts the given bitmap to a DFB.
    // Returns true if the conversion took place,
    // false if the conversion either unneeded or unavailable
    bool ConvertToDFB(HBITMAP& hBitmap);
    template <class T> void SafeRelease(T **ppT);
};

