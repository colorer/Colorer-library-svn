
rem Self runnable version of Colorer library JNI interface
rem Uses SWT API.

rem To use it, please provide full path to colorer/catalog.xml file
rem in %HOME%/.colorer5catalog  file.

set ECLIPSEHOME=../..

rem set ECLIPSEVER=2.1.0
set ECLIPSEVER=2.0.2

set SWT_JAR=%ECLIPSEHOME%\plugins\org.eclipse.swt.win32_%ECLIPSEVER%\ws\win32\swt.jar
set SWT_LIB=%ECLIPSEHOME%\plugins\org.eclipse.swt.win32_%ECLIPSEVER%\os\win32\x86

set classpath=colorer.jar;%SWT_JAR%;%classpath%
set path=./os/win32/x86;%SWT_LIB%;%path%

java net.sf.colorer.swt.ColorerDemo