@ECHO OFF
SET ADB=C:\Users\ga78xox\AppData\Local\Android\sdk\platform-tools\adb.exe

SET MODEL_DIR=C:\Users\ga78xox\Desktop\eCARus_SS15
SET TARGET_DIR=/storage/sdcard0/Android/data/com.example.gtflauncher/files

del %MODEL_DIR%\*.a*
del %MODEL_DIR%\*.h
del %MODEL_DIR%\*.dll
del %MODEL_DIR%\*.lib
del %MODEL_DIR%\*.log
del /Q %MODEL_DIR%\gestures\*
del /Q %MODEL_DIR%\gestures
del /Q %MODEL_DIR%\monitoring\*
del /Q %MODEL_DIR%\monitoring

%ADB% push %MODEL_DIR% %TARGET_DIR%

pause