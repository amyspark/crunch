rem Exports the core of the MSVC environment to the GitHub Actions environment block
echo ::set-env name=CommandPromptType::%CommandPromptType%
echo ::set-env name=DevEnvDir::%DevEnvDir%
echo ::set-env name=ExtensionSdkDir::%ExtensionSdkDir%
echo ::set-env name=INCLUDE::%INCLUDE%
echo ::set-env name=LIB::%LIB%
echo ::set-env name=LIBPATH::%LIBPATH%
echo ::set-env name=PATH::C:\Program Files\OpenCppCoverage;%PATH%
echo ::set-env name=Platform::%Platform%
echo ::set-env name=UCRTVersion::%UCRTVersion%
echo ::set-env name=UniversalCRTSdkDir::%UniversalCRTSdkDir%
echo ::set-env name=VCINSTALLDIR::%VCINSTALLDIR%
echo ::set-env name=VCToolsInstallDir::%VCToolsInstallDir%
echo ::set-env name=VCToolsRedistDir::%VCToolsRedistDir%
echo ::set-env name=VCToolsVersion::%VCToolsVersion%
echo ::set-env name=VisualStudioVersion::%VisualStudioVersion%
echo ::set-env name=VSINSTALLDIR::%VSINSTALLDIR%
echo ::set-env name=WindowsLibPath::%WindowsLibPath%
echo ::set-env name=WindowsSdkBinPath::%WindowsSdkBinPath%
echo ::set-env name=WindowsSdkDir::%WindowsSdkDir%
echo ::set-env name=WindowsSDKLibVersion::%WindowsSDKLibVersion%
echo ::set-env name=WindowsSdkVerBinPath::%WindowsSdkVerBinPath%
echo ::set-env name=WindowsSDKVersion::%WindowsSDKVersion%
echo ::set-env name=WindowsSDK_ExecutablePath_x64::%WindowsSDK_ExecutablePath_x64%
echo ::set-env name=WindowsSDK_ExecutablePath_x86::%WindowsSDK_ExecutablePath_x86%
echo ::set-env name=CPU_COUNT::%NUMBER_OF_PROCESSORS%
