#pragma once
#include <stdarg.h>

#include "pxr/base/tf/debug.h"
#include <pxr/base/tf/diagnostic.h>


TF_DEBUG_CODES(
	USDMAR
);


#define USDMAR_DEBUG(...) TF_DEBUG(USDMAR).Msg("usdmar - " __VA_ARGS__)
#define USDMAR_WARN(...) TF_WARN( TF_FUNC_NAME() +  __VA_ARGS__)
