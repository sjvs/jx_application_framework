/******************************************************************************
 jXConstants.h

	Useful constants

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jXConstants
#define _H_jXConstants

#include <JFontStyle.h>
#include <X11/X.h>

const JCoordinate kJXDefaultBorderWidth = 2;

const Time kJXDoubleClickTime = 500;	// milliseconds

// Since 24-bit color is the maximum, we should never get indicies
// that are this large.

const JColorIndex kJXTransparentColor = kJIndexMax;		// illegal except for JXImageMask

#endif
