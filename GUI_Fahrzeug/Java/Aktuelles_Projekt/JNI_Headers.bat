SET JAVAH="C:\Program Files\Java\jdk1.8.0_60\bin\javah"
SET CLASSPTH=D:\Projects\eCARus\160302\eCARus-IT-master\GUI_Fahrzeug\Fahrzeug_eCARus_Auto_Java\app\build\intermediates\classes\debug

REM Tower JNI Access
SET PACKAGE=com.example.gtflauncher.tower
SET CLASS=TowerJNIAccess
SET OUTFILENAME=D:\Projects\eCARus\160302\Applikationen\code\TowerAccess\src\TowerAccessJNISignatures.h
%JAVAH% -o %OUTFILENAME% -classpath %CLASSPTH% %PACKAGE%.%CLASS%

REM Music Service
SET PACKAGE=com.example.gtflauncher.music
SET CLASS=MusicJNIAccess
SET OUTFILENAME=D:\Projects\eCARus\160302\Applikationen\code\MusicServiceAccess\src\MusicServiceAccessJNISignatures.h
%JAVAH% -o %OUTFILENAME% -classpath %CLASSPTH% %PACKAGE%.%CLASS%