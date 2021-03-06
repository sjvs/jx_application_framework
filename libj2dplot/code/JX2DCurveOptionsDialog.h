/******************************************************************************
 JX2DCurveOptionsDialog.h

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JX2DCurveOptionsDialog
#define _H_JX2DCurveOptionsDialog

#include <JXDialogDirector.h>
#include <J2DCurveInfo.h>

class JString;
class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;
class JXTextRadioButton;
class JX2DCurveNameList;

class JX2DCurveOptionsDialog : public JXDialogDirector
{
public:

	enum
	{
		kShowPoints = 0,
		kShowSymbols,
		kShowLines,
		kShowBoth
	};

public:

	JX2DCurveOptionsDialog(JXWindowDirector* supervisor,
							const JArray<J2DCurveInfo>& array,
							const JArray<JBoolean>& hasXErrors,
							const JArray<JBoolean>& hasYErrors,
							const JArray<JBoolean>& isFunction,
							const JArray<JBoolean>& isScatter,
							const JIndex startIndex);

	virtual ~JX2DCurveOptionsDialog();

	const JArray<J2DCurveInfo>&	GetCurveInfoArray();

protected:

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	JX2DCurveNameList*		itsNameList;		// not owned
	JArray<J2DCurveInfo>*	itsCurveInfo;
	JArray<JBoolean>*		itsHasXErrors;
	JArray<JBoolean>*		itsHasYErrors;
	JArray<JBoolean>*		itsIsFunction;
	JArray<JBoolean>*		itsIsScatter;
	JIndex					itsCurrentIndex;

// begin JXLayout

	JXRadioGroup*      itsCurveStyleRG;
	JXTextCheckbox*    itsShowCurveCB;
	JXTextRadioButton* itsShowLinesRB;
	JXTextRadioButton* itsShowBothRB;
	JXTextRadioButton* itsShowSymbolsRB;
	JXTextRadioButton* itsShowPointsRB;
	JXTextButton*      itsCancelButton;
	JXTextCheckbox*    itsShowXErrorsCB;
	JXTextCheckbox*    itsShowYErrorsCB;

// end JXLayout

private:

	void	BuildWindow();
	void	SaveSettings();
	void	AdjustWidgets();
	JIndex	EncodeCurveStyle(const J2DCurveInfo& info);
	void	DecodeCurveStyle(J2DCurveInfo* info);

	// not allowed

	JX2DCurveOptionsDialog(const JX2DCurveOptionsDialog& source);
	const JX2DCurveOptionsDialog& operator=(const JX2DCurveOptionsDialog& source);
};


/******************************************************************************
 GetCurveInfoArray

 ******************************************************************************/

inline const JArray<J2DCurveInfo>&
JX2DCurveOptionsDialog::GetCurveInfoArray()
{
	return *itsCurveInfo;
}

#endif
