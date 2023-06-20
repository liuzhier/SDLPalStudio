Copy /y ucrtbased.dll %windir%\system32\
regsvr32/s %windir%\system32\ucrtbased.dll

Copy /y vcruntime140d.dll %windir%\system32\
regsvr32/s %windir%\system32\vcruntime140d.dll