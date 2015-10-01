// dllmain.h : Declaration of module class.

class CMusicianHelperNativeVideoEncoderModule : public ATL::CAtlDllModuleT< CMusicianHelperNativeVideoEncoderModule >
{
public :
	DECLARE_LIBID(LIBID_MusicianHelperNativeVideoEncoderLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MUSICIANHELPERNATIVEVIDEOENCODER, "{62898641-30C5-4F69-AD61-83461634F046}")
};

extern class CMusicianHelperNativeVideoEncoderModule _AtlModule;
