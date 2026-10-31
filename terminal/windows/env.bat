@REM  @for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do @set "VS_HOME=%%i"
@for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -products * -property installationPath`) do @set "VS_HOME=%%i"

@REM Output a powershell script.
@echo $VS_HOME="%VS_HOME%"