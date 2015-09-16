@echo off

SET INSTALLERDIR=%CD%
cd ..\
SET BASEDIR=%CD%
cd %INSTALLERDIR%

rmdir /S /Q InstallerData
mkdir InstallerData
cd InstallerData
SET INSTALLERDATADIR=%CD%

cd %BASEDIR%

xcopy *.config %INSTALLERDATADIR% /Q /Y
xcopy api_keys.json %INSTALLERDATADIR% /Q /Y
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.exe" MusicianHelper.sln /Build "Release"
cd MusicianHelper.WinForms\bin\Release
xcopy *.* %INSTALLERDATADIR% /Q /Y
cd %BASEDIR%\MusicianHelper.APIKeyBootstrapper\bin\Release
xcopy *.exe %INSTALLERDATADIR% /Q /Y

cd %INSTALLERDATADIR%
del *.pdb
start MusicianHelper.APIKeyBootstrapper

cd %INSTALLERDIR%