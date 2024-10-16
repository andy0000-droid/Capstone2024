#pragma once
#include <windows.h>
#define DLL_PROC_SETUP(M) BOOL _dllprocloaded = TRUE; HMODULE _dllmodule = M;
#define DLL_PROC_LOAD(VAR, LOAD_NAME, LOAD_TYPE) (_dllprocloaded = _dllprocloaded && ((VAR = (LOAD_TYPE)GetProcAddress(_dllmodule, LOAD_NAME)) != NULL))
#define DLL_PROC_RESULT(LOADED) (LOADED = _dllprocloaded)