// VideoEncoderWrapper.cpp : Implementation of CVideoEncoderWrapper

#include "stdafx.h"
#include "VideoEncoderWrapper.h"


// CVideoEncoderWrapper



STDMETHODIMP CVideoEncoderWrapper::get_ImageFilePath(LPCWSTR* pVal)
{
    LPCWSTR *test = new LPCWSTR;
    *test = L"Test string";
    pVal = test;

    return S_OK;
}


STDMETHODIMP CVideoEncoderWrapper::put_ImageFilePath(LPCWSTR newVal)
{
    mImageFilePath = newVal;

    return S_OK;
}
