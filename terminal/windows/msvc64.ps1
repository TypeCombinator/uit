$EnvContent = cmd /c $PSScriptRoot\env.bat
Invoke-Expression $EnvContent

Import-Module "$VS_HOME\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -VsInstallPath "$VS_HOME" -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64'