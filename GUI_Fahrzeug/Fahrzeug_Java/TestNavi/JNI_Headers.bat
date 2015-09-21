SET JAVAH="C:\Program Files\Java\jdk1.8.0_60\bin\javah"
SET CLASSPTH=D:\Projects\eCARus\Aktuell\TestNavi\app\build\intermediates\classes\debug\

REM Tower JNI Access
SET PACKAGE=com.example.gtflauncher.tower
SET CLASS=TowerJNIAccess
SET OUTFILENAME=D:\Projects\eCARus\Aktuell\eCARusApps\TowerAccess\src\TowerAccessJNISignatures.h
%JAVAH% -o %OUTFILENAME% -classpath %CLASSPTH% %PACKAGE%.%CLASS%

REM Navigation JNI Access
SET PACKAGE=com.example.gtflauncher.navigation
SET CLASS=NavigationJNIAccess
SET OUTFILENAME=D:\Projects\eCARus\Aktuell\eCARusApps\NavigationServiceAccess\src\NavigationAccessJNISignatures.h
REM %JAVAH% -o %OUTFILENAME% -classpath %CLASSPTH% %PACKAGE%.%CLASS%