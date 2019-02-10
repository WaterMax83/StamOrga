@echo off
set NewFolderPath="E:\Users\WaterMax\Documents\Projekte\Qt\StamOrga\versions\StamOrga.Winx64.v1.1.4"

cd \
c:
rmdir %NewFolderPath% /s /q
mkdir %NewFolderPath%
xcopy "E:\Users\WaterMax\Documents\Projekte\Qt\build\build-StamOrga-Desktop_Qt_5_9_2_MSVC2017_64bit-Release\release\StamOrga.exe" %NewFolderPath%
xcopy "E:\Users\WaterMax\Documents\Projekte\Qt\StamOrga\versions\libeay32.dll" %NewFolderPath%
xcopy "E:\Users\WaterMax\Documents\Projekte\Qt\StamOrga\versions\ssleay32.dll" %NewFolderPath%

cd Qt\Qt5.9.2\5.9.2\msvc2017_64\bin
windeployqt.exe %NewFolderPath% --qmldir "E:\Users\WaterMax\Documents\Projekte\Qt\StamOrga\StamOrga\Android"

pause
