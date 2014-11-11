cl /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MD /Zi /Ob0 /Od /RTC1 -I..\lib\win32\include -I..\include -I..\src /wd4996 /D _CRT_SECURE_NO_WARNINGS /TP -c ..\bin\generate_fpkeys.cpp
cl /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MD /Zi /Ob0 /Od /RTC1 -I..\lib\win32\include -I..\include -I..\src /wd4996 /D _CRT_SECURE_NO_WARNINGS /TP -c ..\src\asearch.cpp
cl /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MD /Zi /Ob0 /Od /RTC1 -I..\lib\win32\include -I..\include -I..\src /wd4996 /D _CRT_SECURE_NO_WARNINGS /TP -c ..\src\utils.cpp
cl /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MD /Zi /Ob0 /Od /RTC1 -I..\lib\win32\include -I..\include -I..\src /wd4996 /D _CRT_SECURE_NO_WARNINGS /TP -c ..\src\sigproc.cpp
cl /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /MD /Zi /Ob0 /Od /RTC1 -I..\lib\win32\include -I..\include -I..\src /wd4996 /D _CRT_SECURE_NO_WARNINGS /TP -c ..\src\filters.cpp
link generate_fpkeys.obj asearch.obj utils.obj sigproc.obj filters.obj /out:generate_fpkeys.exe /subsystem:console /libpath:..\lib\win32 libfftw3-3.lib libsndfile-1.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib  
del *.obj *.pdb

