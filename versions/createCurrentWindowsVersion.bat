@echo off
set NewFolderPath="D:\Daten\Projekte\StamOrga\versions\StamOrga.Winx64.V1.1.X"

cd \
c:
rmdir %NewFolderPath% /s /q
mkdir %NewFolderPath%
xcopy "D:\Daten\Projekte\StamOrga\build\build-StamOrga-Desktop_Qt_5_9_2_MSVC2017_64bit-Release\release\StamOrga.exe" %NewFolderPath%
xcopy "C:\OpenSSL-Win64\bin\libeay32.dll" %NewFolderPath%
xcopy "C:\OpenSSL-Win64\bin\ssleay32.dll" %NewFolderPath%

cd Qt\Qt5.9.2\5.9.2\msvc2017_64\bin
windeployqt.exe %NewFolderPath% --qmldir "D:\Daten\Projekte\StamOrga\StamOrga\Android"

pause
