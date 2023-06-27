#ifndef SCRIPT_H
#define SCRIPT_H

#include "main.h"

PAL_C_LINKAGE_BEGIN

LPSTR
PAL_GetScriptMessage(
	LPSCRIPTENTRY lprgScriptEntry,
	LPSTR strBuffer
);

WORD
PAL_GetScriptLen(
	WORD wStartScriptAddress,
	WORD wRoundOfJump
);

PAL_C_LINKAGE_END

#endif  // SCRIPT_H