rem @echo off

cd InstallerData

rmdir /S /Q errorlogs
del MusicianHelper.APIKeyBootstrapper.exe
del log4net.Debug.config

cd ..\

rem candle installer.wxs
rem light installer.wixobj

@pause