/******************************************************************************
 GLFitDescription.h

	Interface for the GLFitDescription class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GLFitDescription
#define _H_GLFitDescription

#include "GLFitBase.h"
#include "GLVarList.h"
#include <JArray.h>
#include <JString.h>

class GLFitDescription : public GLFitBase
{
public:

	enum FitType
	{
		kPolynomial = 1,
		kNonLinear,
		kModule,
		kBLinear,
		kBQuadratic,
		kBExp,
		kBPower
	};

public:

	GLFitDescription(const FitType type, const JString& form = JString::empty,
					 const JString& name = JString::empty);

	static JBoolean	Create(std::istream& is, GLFitDescription** fd);
	
	virtual ~GLFitDescription();

	FitType	GetType() const;
	void	SetType(const FitType type); 

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const;


	virtual JString	GetFitFunctionString() const;
	void			SetFitFunctionString(const JString& form);

	const JString&	GetFnName() const;

	JBoolean		RequiresStartValues() const;
	JBoolean		CanUseStartValues() const;

	GLVarList*		GetVarList();

	virtual void	WriteSetup(std::ostream& os); // must call base class first!

	static JListT::CompareResult
		CompareFits(GLFitDescription * const &, GLFitDescription * const &);

protected:

	void			SetFnName(const JString& name);
	void			DoesRequireStartValues(const JBoolean require);
	void			SetCanUseStartValues(const JBoolean use);

private:

	GLVarList*	itsVarList;
	FitType		itsType;
	JString		itsFnForm;
	JString		itsFnName;
	JBoolean	itsRequiresStartValues;
	JBoolean	itsCanUseStartValues;
	
private:

	// not allowed

	GLFitDescription(const GLFitDescription& source);
	const GLFitDescription& operator=(const GLFitDescription& source);

};

/******************************************************************************
 GetType (public)

 ******************************************************************************/

inline GLFitDescription::FitType
GLFitDescription::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

inline JString
GLFitDescription::GetFitFunctionString()
	const
{
	return itsFnForm;
}

/******************************************************************************
 GetFnName (public)

 ******************************************************************************/

inline const JString&
GLFitDescription::GetFnName()
	const
{
	return itsFnName;
}

/******************************************************************************
 RequiresStartValues (public)

 ******************************************************************************/

inline JBoolean
GLFitDescription::RequiresStartValues()
	const
{
	return itsRequiresStartValues;
}

/******************************************************************************
 CanUseStartValues (public)

 ******************************************************************************/

inline JBoolean
GLFitDescription::CanUseStartValues()
	const
{
	return itsCanUseStartValues;
}

/******************************************************************************
 GetVarList (public)

 ******************************************************************************/

inline GLVarList*
GLFitDescription::GetVarList()
{
	return itsVarList;
}

#endif
