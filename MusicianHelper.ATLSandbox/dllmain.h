// dllmain.h : Declaration of module class.

class CMusicianHelperATLSandboxModule : public ATL::CAtlDllModuleT< CMusicianHelperATLSandboxModule >
{
public :
	DECLARE_LIBID(LIBID_MusicianHelperATLSandboxLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MUSICIANHELPERATLSANDBOX, "{AC59EC47-2BCE-4025-B4D4-E5FAEA12E92A}")
};

extern class CMusicianHelperATLSandboxModule _AtlModule;
