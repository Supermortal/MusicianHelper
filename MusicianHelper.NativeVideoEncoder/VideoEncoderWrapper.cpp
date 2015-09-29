// VideoEncoderWrapper.cpp : Implementation of CVideoEncoderWrapper

#include "stdafx.h"
#include "VideoEncoderWrapper.h"


// CVideoEncoderWrapper

class VideoEncoderWrapper : CVideoEncoderWrapper {
    VideoEncoderWrapper() {

    }
    ~VideoEncoderWrapper() {

    }
};



STDMETHODIMP CVideoEncoderWrapper::Encode()
{
    

    return S_OK;
}

STDMETHODIMP CVideoEncoderWrapper::put_ImageFilePath(LPCWSTR newVal)
{
    // TODO: Add your implementation code here

    return S_OK;
}


STDMETHODIMP CVideoEncoderWrapper::put_AudioFilePath(LPCWSTR newVal)
{
    // TODO: Add your implementation code here

    return S_OK;
}


STDMETHODIMP CVideoEncoderWrapper::put_VideoOutputPath(LPCWSTR newVal)
{
    // TODO: Add your implementation code here

    return S_OK;
}
