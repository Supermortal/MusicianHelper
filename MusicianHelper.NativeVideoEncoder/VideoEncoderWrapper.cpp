// VideoEncoderWrapper.cpp : Implementation of CVideoEncoderWrapper

#include "stdafx.h"
#include "VideoEncoderWrapper.h"


// CVideoEncoderWrapper



STDMETHODIMP CVideoEncoderWrapper::get_ImageFilePath(LPCWSTR* pVal)
{
    *pVal = L"Test string";

    return S_OK;
}


STDMETHODIMP CVideoEncoderWrapper::put_ImageFilePath(LPCWSTR newVal)
{
    mImageFilePath = newVal;

    return S_OK;
}
