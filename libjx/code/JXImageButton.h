/******************************************************************************
 JXImageButton.h

	Interface for the JXImageButton class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageButton
#define _H_JXImageButton

#include <JXButton.h>
#include <jXConstants.h>		// need definition of kJXTransparentColor
#include <JConstBitmap.h>
#include <JXPM.h>

class JXImage;

class JXImageButton : public JXButton
{
public:

	JXImageButton(JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~JXImageButton();

	void	SetBitmap(const JConstBitmap& bitmap,
					  const JColorIndex foreColor = kJXTransparentColor,  // placeholder for GetBlackColor()
					  const JColorIndex backColor = kJXTransparentColor); // placeholder for GetDefaultBackColor()
	void	SetImage(const JXPM& xpm,
					 const JColorIndex backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()
	void	SetImage(JXImage* image, const JBoolean widgetOwnsImage,
					 const JColorIndex backColor = kJXTransparentColor);  // placeholder for GetDefaultBackColor()

	void	FitToImage();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

private:

	JXImage*	itsImage;
	JBoolean	itsOwnsImageFlag;

private:

	// not allowed

	JXImageButton(const JXImageButton& source);
	const JXImageButton& operator=(const JXImageButton& source);
};

#endif
