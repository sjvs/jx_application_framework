/******************************************************************************
 JXFontNameMenu.h

	Interface for the JXFontNameMenu class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFontNameMenu
#define _H_JXFontNameMenu

#include "JXTextMenu.h"

class JXFontNameMenu : public JXTextMenu
{
public:

	JXFontNameMenu(const JString& title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	JXFontNameMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXFontNameMenu();

	JString		GetFontName() const;
	JBoolean	SetFontName(const JString& name);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

	void	SetToPopupChoice(const JBoolean isPopup = kJTrue);

#pragma GCC diagnostic pop

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex		itsFontIndex;
	JBoolean	itsBroadcastNameChangeFlag;

private:

	void	BuildMenu();

	// not allowed

	JXFontNameMenu(const JXFontNameMenu& source);
	const JXFontNameMenu& operator=(const JXFontNameMenu& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNameNeedsUpdate;
	static const JUtf8Byte* kNameChanged;

	class NameNeedsUpdate : public JBroadcaster::Message
		{
		public:

			NameNeedsUpdate()
				:
				JBroadcaster::Message(kNameNeedsUpdate)
				{ };
		};

	class NameChanged : public JBroadcaster::Message
		{
		public:

			NameChanged()
				:
				JBroadcaster::Message(kNameChanged)
				{ };

		// call broadcaster's GetFont() to get fontName
		};
};


/******************************************************************************
 GetFontName

 ******************************************************************************/

inline JString
JXFontNameMenu::GetFontName()
	const
{
	return GetItemText(itsFontIndex);
}

#endif
