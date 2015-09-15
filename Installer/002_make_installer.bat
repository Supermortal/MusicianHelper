@echo off

cd InstallerData

rmdir /S /Q errorlogs
del MusicianHelper.APIKeyBootstrapper.exe
del log4net.Debug.config

cls
cd ..\

candle installer.wxs
light installer.wixobj

@pause