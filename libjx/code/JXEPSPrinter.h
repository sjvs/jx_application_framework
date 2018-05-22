/******************************************************************************
 JXEPSPrinter.h

	Interface for the JXEPSPrinter class

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEPSPrinter
#define _H_JXEPSPrinter

#include <JEPSPrinter.h>

class JXDisplay;
class JXImage;
class JXImagePainter;
class JXEPSPrintSetupDialog;

class JXEPSPrinter : public JEPSPrinter
{
public:

	JXEPSPrinter(JXDisplay* display);

	virtual ~JXEPSPrinter();

	// saving setup information

	void	ReadXEPSSetup(std::istream& input);
	void	WriteXEPSSetup(std::ostream& output) const;

	// preview

	virtual JPainter&	GetPreviewPainter(const JRect& bounds) override;

	// Print Setup dialog

	void	BeginUserPrintSetup();

protected:

	virtual JBoolean	GetPreviewImage(const JImage** image) const override;
	virtual void		DeletePreviewData() override;

	virtual JXEPSPrintSetupDialog*
		CreatePrintSetupDialog(const JString& fileName,
							   const JBoolean preview, const JBoolean bw);

	virtual JBoolean	EndUserPrintSetup(const JBroadcaster::Message& message,
										  JBoolean* changed);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDisplay*	itsDisplay;		// not owned

	JXImage*		itsPreviewImage;
	JXImagePainter*	itsPreviewPainter;

	JXEPSPrintSetupDialog*	itsPrintSetupDialog;

private:

	// not allowed

	JXEPSPrinter(const JXEPSPrinter& source);
	const JXEPSPrinter& operator=(const JXEPSPrinter& source);
};

#endif
