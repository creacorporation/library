@echo off

rem ----- ライブラリ有効性チェック -----
if not exist "packages\Microsoft.Web.WebView2.*" goto libcheck1
echo #define %1_ENABLE_WEBVIEW2
:libcheck1

rem ----- SVNリビジョンチェック -----
svn help > nul
if errorlevel 9009 goto errend

svn info . | findstr "Revision:" > tmp_svnversion.txt
for /F "DELIMS=" %%t in ( tmp_svnversion.txt ) do set SVNREVISION=%%t
set SVNREVISION=%SVNREVISION:~10%
set SVNREVISIONSTR=%SVNREVISION%
set SVNMODIFIED=0
del tmp_svnversion.txt

svn st . | findstr "^[MA!]" > nul
if errorlevel 1 goto end
set SVNREVISIONSTR=%SVNREVISION%-MOD
set SVNMODIFIED=1

:end
echo #define %1_REVISION_NUMBER %SVNREVISION%
echo #define %1_REVISION_STRING "%SVNREVISIONSTR%"
echo #define %1_REVISION_MODIFIED %SVNMODIFIED%
echo #define %1_REVISION_EXPORTED 0
set SVNREVISION=
set REVISION_STRING=
set REVISION_MODIFIED=
exit /B 0

:errend
echo #define %1_REVISION_NUMBER 0
echo #define %1_REVISION_STRING "EXPORTED"
echo #define %1_REVISION_MODIFIED 0
echo #define %1_REVISION_EXPORTED 1
exit /B 0

