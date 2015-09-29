// dllmain.h : Declaration of module class.

class CMusicianHelperNativeVideoEncoderModule : public ATL::CAtlDllModuleT< CMusicianHelperNativeVideoEncoderModule >
{
public :
	DECLARE_LIBID(LIBID_MusicianHelperNativeVideoEncoderLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MUSICIANHELPERNATIVEVIDEOENCODER, "{679561C1-D2E6-4D83-8B60-64E84F79E8FB}")
};

extern class CMusicianHelperNativeVideoEncoderModule _AtlModule;
