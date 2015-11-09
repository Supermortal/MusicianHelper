// dllmain.h : Declaration of module class.

class CMusicianHelperNativeVideoEncoderModule : public ATL::CAtlDllModuleT< CMusicianHelperNativeVideoEncoderModule >
{
public :
	DECLARE_LIBID(LIBID_MusicianHelperNativeVideoEncoderLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MUSICIANHELPERNATIVEVIDEOENCODER, "{6E93CAD9-237D-4B29-AB32-B67BCD859E08}")
};

extern class CMusicianHelperNativeVideoEncoderModule _AtlModule;
