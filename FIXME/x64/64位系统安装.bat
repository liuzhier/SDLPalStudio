Copy /y ucrtbased.dll %windir%\SysWOW64
regsvr32 %windir%\SysWOW64\ucrtbased.dll

Copy /y vcruntime140d.dll %windir%\SysWOW64
regsvr32 %windir%\SysWOW64\vcruntime140d.dll