/******************************************************************************
 JContainer.h

	Interface for the JContainer class

	Copyright (C) 1994 John Lindal.

 ******************************************************************************/

#ifndef _H_JContainer
#define _H_JContainer

#include "JCollection.h"

class JContainer : public JCollection
{
public:

	JContainer();
	JContainer(const JContainer& source);

	const JContainer& operator=(const JContainer& source);

protected:

	void				InstallCollection(JCollection* list);
	const JCollection*	GetList();

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JCollection*	itsList;
};


/******************************************************************************
 GetList

 ******************************************************************************/

inline const JCollection*
JContainer::GetList()
{
	return itsList;
}

#endif
