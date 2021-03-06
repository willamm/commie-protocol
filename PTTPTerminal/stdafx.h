// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <commdlg.h>
#include <minwinbase.h>
#include <Commctrl.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <exception>


// TODO: reference additional headers your program requires here
#include "DataFrame.h"
#include "OpenFile.h"
#include "StatusBar.h"
#include "SerialPortConfig.h"