// VideoEncoder.cpp : Implementation of CVideoEncoder

#include "stdafx.h"
#include "VideoEncoder.h"


// CVideoEncoder

STDMETHODIMP CVideoEncoder::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_IVideoEncoder
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CVideoEncoder::get_ImageFilePath(BSTR* pVal)
{
    _bstr_t bstr1(mImageFilePath);
    BSTR bstr;

    bstr = bstr1.copy();
    *pVal = bstr;

    return S_OK;
}


STDMETHODIMP CVideoEncoder::put_ImageFilePath(BSTR newVal)
{
    _bstr_t bstrStart(newVal);
    mImageFilePath = (LPCWSTR)bstrStart;

    return S_OK;
}


STDMETHODIMP CVideoEncoder::get_VideoOutputPath(BSTR* pVal)
{
    _bstr_t bstr1(mVideoOutputPath);
    BSTR bstr;

    bstr = bstr1.copy();
    *pVal = bstr;

    return S_OK;
}


STDMETHODIMP CVideoEncoder::put_VideoOutputPath(BSTR newVal)
{
    _bstr_t bstrStart(newVal);
    mVideoOutputPath = (LPCWSTR)bstrStart;

    return S_OK;
}


STDMETHODIMP CVideoEncoder::get_AudioFilePath(BSTR* pVal)
{
    _bstr_t bstr1(mAudioFilePath);
    BSTR bstr;

    bstr = bstr1.copy();
    *pVal = bstr;

    return S_OK;
}


STDMETHODIMP CVideoEncoder::put_AudioFilePath(BSTR newVal)
{
    _bstr_t bstrStart(newVal);
    mAudioFilePath = (LPCWSTR)bstrStart;

    return S_OK;
}
