rem @echo off

rem ----- ���C�u�����L�����`�F�b�N -----
if not exist %2libconfig.conf goto optcheckend

rem WebView2�g�����ǂ����̔���
find /I "ENABLE_WEBVIEW2" %2libconfig.conf > nul
if errorlevel 1 goto disable_webview2
echo #define %1_ENABLE_WEBVIEW2
:disable_webview2

rem CRTDBG�g�����ǂ����̔���
find /I "ENABLE_CRTDBG" %2libconfig.conf > nul
if errorlevel 1 goto disable_crtdbg
echo #define %1_ENABLE_CRTDBG
:disable_crtdbg

rem EXCEL�g�����ǂ����̔���
find /I "ENABLE_EXCEL" %2libconfig.conf > nul
if errorlevel 1 goto disable_excel
echo #define %1_ENABLE_EXCEL
:disable_excel


:optcheckend

rem ----- SVN���r�W�����`�F�b�N -----
git help > nul
if errorlevel 9009 goto errend

git show --format='%%h' --no-patch > tmp_svnversion.txt
for /F "DELIMS=" %%t in ( tmp_svnversion.txt ) do set SVNREVISION=%%t
set SVNREVISION=%SVNREVISION:~1,7%
set SVNREVISIONSTR=%SVNREVISION%

git show --format='%%H' --no-patch > tmp_svnversion.txt
for /F "DELIMS=" %%t in ( tmp_svnversion.txt ) do set SVNREVISIONLONGSTR=%%t
set SVNREVISIONLONGSTR=%SVNREVISIONLONGSTR:~1,40%

del tmp_svnversion.txt

:end
echo #define %1_REVISION_NUMBER 0x%SVNREVISION%u
echo #define %1_REVISION_STRING "%SVNREVISIONSTR%"
echo #define %1_REVISION_STRING_LONG "%SVNREVISIONLONGSTR%"
echo #define %1_REVISION_MODIFIED 0
echo #define %1_REVISION_EXPORTED 0
set SVNREVISION=
set SVNREVISIONSTR=
set SVNREVISIONLONGSTR=
exit /B 0

:errend
echo #define %1_REVISION_NUMBER 0
echo #define %1_REVISION_STRING "EXPORTED"
echo #define %1_REVISION_STRING_LONG "EXPORTED"
echo #define %1_REVISION_MODIFIED 0
echo #define %1_REVISION_EXPORTED 1
exit /B 0

