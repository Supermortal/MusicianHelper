// VideoEncoder.h : Declaration of the CVideoEncoder

/*http://stackoverflow.com/questions/295067/passing-an-array-using-com*/
/*http://www.codeproject.com/Articles/4829/Guide-to-BSTR-and-C-String-Conversions*/

#pragma once
#include "resource.h"       // main symbols



#include "MusicianHelperNativeVideoEncoder_i.h"
#include "_IVideoEncoderEvents_CP.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CVideoEncoder

class ATL_NO_VTABLE CVideoEncoder :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVideoEncoder, &CLSID_VideoEncoder>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CVideoEncoder>,
	public CProxy_IVideoEncoderEvents<CVideoEncoder>,
	public IDispatchImpl<IVideoEncoder, &IID_IVideoEncoder, &LIBID_MusicianHelperNativeVideoEncoderLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CVideoEncoder()
	{
        mVideoBitRate = 800000;
        mVideoFps = 60;
        mVideoEncodingFormat = MFVideoFormat_WMV3;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_VIDEOENCODER)


BEGIN_COM_MAP(CVideoEncoder)
	COM_INTERFACE_ENTRY(IVideoEncoder)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CVideoEncoder)
	CONNECTION_POINT_ENTRY(__uuidof(_IVideoEncoderEvents))
END_CONNECTION_POINT_MAP()
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

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
    HBITMAP LoadImageFromFilePath(LPCWSTR filePath);
    int GetDIBFromHandle(HBITMAP hBitmap, BITMAP *bitmap);
    void SetVideoHeightAndWidth(BITMAP bitmap);
    HRESULT StartMediaFoundation();
    HRESULT CreateMediaSource(
        PCWSTR pszURL,
        IPropertyStore *pConfig,    // Optional, can be NULL
        IMFMediaSource **ppSource
        );
    HRESULT GetSourceDuration(IMFMediaSource *pSource, MFTIME *pDuration);
    template <class T> void SafeRelease(T **ppT);
    UINT64 GetVideoFrameCount();
    UINT64 GetVideoFrameDuration();
    HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex, DWORD *pAudioStreamIndex, LPCWSTR videoOutputPath, IMFSourceReader *pReader);
    HRESULT WriteFrame(
        IMFSinkWriter *pWriter,
        DWORD streamIndex,
        const LONGLONG& rtStart,        // Time stamp.
        byte* vfb
        );
    bool ConvertToDIB(HBITMAP& hBitmap);
public:
    STDMETHOD(get_ImageFilePath)(BSTR* pVal);
    STDMETHOD(put_ImageFilePath)(BSTR newVal);
    STDMETHOD(get_VideoOutputPath)(BSTR* pVal);
    STDMETHOD(put_VideoOutputPath)(BSTR newVal);
    STDMETHOD(get_AudioFilePath)(BSTR* pVal);
    STDMETHOD(put_AudioFilePath)(BSTR newVal);
    STDMETHOD(Encode)();
};

OBJECT_ENTRY_AUTO(__uuidof(VideoEncoder), CVideoEncoder)
