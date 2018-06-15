/******************************************************************************
 JXDockWidget.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockWidget
#define _H_JXDockWidget

#include "JXWidgetSet.h"

class JXWindowDirector;
class JXDockDirector;
class JXPartition;
class JXHorizDockPartition;
class JXVertDockPartition;
class JXTabGroup;
class JXHintDirector;
class JXTimerTask;

class JXDockWidget : public JXWidgetSet
{
public:

	enum
	{
		kDefaultMinSize = 10
	};

public:

	JXDockWidget(JXDockDirector* director, JXPartition* partition,
				 const JBoolean isHorizPartition, JXTabGroup* tabGroup,
				 JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXDockWidget();

	JXDockDirector*	GetDockDirector() const;
	JXTabGroup*		GetTabGroup() const;
	JPoint			GetMinSize() const;
	void			UpdateMinSize();

	JBoolean	HasWindows() const;
	JBoolean	CloseAllWindows();		// recursive

	JSize		GetWindowCount() const;
	JBoolean	GetWindow(const JIndex index, JXWindow** window);
	void		ShowPreviousWindow();
	void		ShowNextWindow();

	JBoolean	WindowWillFit(JXWindow* w) const;
	JBoolean	Dock(JXWindowDirector* d, const JBoolean reportError = kJFalse);
	JBoolean	Dock(JXWindow* w, const JBoolean reportError = kJFalse);
	JBoolean	TransferAll(JXDockWidget* target);
	void		UndockAll();
	JBoolean	CloseAll();				// not recursive

	JBoolean	GetHorizChildPartition(JXHorizDockPartition** p) const;
	JBoolean	GetVertChildPartition(JXVertDockPartition** p) const;
	void		SetChildPartition(JXPartition* p);

	JIndex	GetID() const;
	void	SetID(const JIndex id);

protected:

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;
	virtual void		HandleDNDLeave() override;

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JXDockDirector*			itsDirector;
	JXPartition*			itsPartition;
	const JBoolean			itsIsHorizFlag;
	JXTabGroup*				itsTabGroup;
	JIndex					itsID;
	JPtrArray<JXWindow>*	itsWindowList;		// nullptr if contains partition
	JPoint					itsMinSize;
	JXPartition*			itsChildPartition;	// nullptr if contains docked windows
	JXHintDirector*			itsHintDirector;	// nullptr unless showing hint; not owned
	JXTimerTask*			itsDeleteHintTask;	// nullptr unless showing hint

private:

	JBoolean	FindWindow(JBroadcaster* sender, JXWindow** window, JIndex* index) const;
	JBoolean	RemoveWindow(JBroadcaster* sender);
	JIndex		GetTabInsertionIndex(JXWindow* w, const JIndex ignoreIndex = 0) const;

	// not allowed

	JXDockWidget(const JXDockWidget& source);
	const JXDockWidget& operator=(const JXDockWidget& source);
};


/******************************************************************************
 GetDockDirector

 ******************************************************************************/

inline JXDockDirector*
JXDockWidget::GetDockDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetTabGroup

 ******************************************************************************/

inline JXTabGroup*
JXDockWidget::GetTabGroup()
	const
{
	return itsTabGroup;
}

/******************************************************************************
 GetID

 ******************************************************************************/

inline JIndex
JXDockWidget::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetWindowCount

 ******************************************************************************/

inline JSize
JXDockWidget::GetWindowCount()
	const
{
	return (itsWindowList == nullptr ? 0 : itsWindowList->GetElementCount());
}

/******************************************************************************
 GetWindow

 ******************************************************************************/

inline JBoolean
JXDockWidget::GetWindow
	(
	const JIndex	index,
	JXWindow**		window
	)
{
	if (itsWindowList == nullptr)
		{
		*window = nullptr;
		return kJFalse;
		}

	*window = itsWindowList->GetElement(index);
	return kJTrue;
}

#endif
