taskkill /f /im �籣�ƿ�*
del /f /q "%HOMEDRIVE%\�籣�ƿ���ʽ�ͻ���"
del "%systemroot%\netetp.dll"
del "%systemroot%\HD300_V1.dll"
RegSvr32 /s -u "%~dp0\lib\soap\MSSOAP30.dll"
RegSvr32 /s -u "%~dp0\lib\soap\Resources\1033\MSSOAPR3.dll"
RegSvr32 /s -u "%~dp0\lib\soap\WISC30.dll"
RegSvr32 /s -u "%~dp0\lib\soap\msxml4.dll"