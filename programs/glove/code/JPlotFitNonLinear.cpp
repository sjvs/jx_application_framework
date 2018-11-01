/******************************************************************************
 JPlotFitNonLinear.cpp

	<Description>

	BASE CLASS = public JPlotFitBase

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include "JPlotFitNonLinear.h"
#include "GVarList.h"
#include <JExprParser.h>
#include <JVector.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JPlotFitNonLinear::JPlotFitNonLinear
	(
	J2DPlotWidget* 	plot, 
	JPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	JPlotFitBase(plot, fitData, xMin, xMax),
	itsVarList(nullptr),
	itsFunction(nullptr),
	itsFPrimed(nullptr),
	itsErrors(nullptr)
{
}

JPlotFitNonLinear::JPlotFitNonLinear
	(
	J2DPlotWidget* plot, 
	JPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	JPlotFitBase(plot, fitData, xmin, xmax, ymin, ymax),
	itsVarList(nullptr),
	itsFunction(nullptr),
	itsFPrimed(nullptr),
	itsErrors(nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JPlotFitNonLinear::~JPlotFitNonLinear()
{
//	jdelete itsVarList; 		we don't own it
	jdelete itsFunction;
	jdelete itsFPrimed;
	jdelete itsErrors;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
JPlotFitNonLinear::SetCurrentParameters
	(
	const JVector& p
	)
{
	assert(itsVarList != nullptr);

	const JSize count	= p.GetDimensionCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsVarList->SetValue(i + 1, p.GetElement(i));
		}
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
JPlotFitNonLinear::SetErrors
	(
	const JVector& p
	)
{
	*itsErrors	= p;
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
JPlotFitNonLinear::FunctionN
	(
	const JFloat x
	)
{
	assert(itsVarList != nullptr);
	assert(itsFunction != nullptr);
	itsVarList->SetValue(1, x);
	JFloat y = 0;;
	itsFunction->Evaluate(&y);
	return y;
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
JPlotFitNonLinear::FunctionNPrimed
	(
	const JFloat x
	)
{
	if (itsFPrimed == nullptr)
		{
		return JPlotFitBase::FunctionNPrimed(x);
		}
	assert(itsVarList != nullptr);
	assert(itsFunction != nullptr);
	itsVarList->SetValue(1, x);
	JFloat y = 0;
	itsFPrimed->Evaluate(&y);
	return y;
}

/******************************************************************************
 SetVarList (public)

 ******************************************************************************/

void
JPlotFitNonLinear::SetVarList
	(
	GVarList* list
	)
{
	itsVarList	= list;
	const JSize count	= list->GetVariableCount() - 1;
	SetParameterCount(count);
	itsErrors	= jnew JVector(count);
	assert(itsErrors != nullptr);
}

/******************************************************************************
 SetFunction (public)

 ******************************************************************************/

void
JPlotFitNonLinear::SetFunction
	(
	const JString& function
	)
{
	assert(itsVarList != nullptr);
	SetFunctionString(function);

	JExprParser p(itsVarList);

	const JBoolean ok = p.Parse(function, &itsFunction);
	assert(ok);
}

/******************************************************************************
 SetFPrimed (public)

 ******************************************************************************/

void
JPlotFitNonLinear::SetFPrimed
	(
	const JString& fPrimed
	)
{
	assert(itsVarList != nullptr);

	JExprParser p(itsVarList);

	const JBoolean ok = p.Parse(fPrimed, &itsFPrimed);
	assert(ok);
}

/******************************************************************************
 SetInitialParameters (public)

 ******************************************************************************/

void
JPlotFitNonLinear::SetInitialParameters
	(
	const JVector& p
	)
{
	GenerateFit(p, 0);
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

JBoolean
JPlotFitNonLinear::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	JPlotFitNonLinear* th	= const_cast< JPlotFitNonLinear* >(this);
	*y	= th->FunctionN(x);
	return kJTrue;
}


/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

JBoolean
JPlotFitNonLinear::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	*name	= itsVarList->GetVariableName(index + 1);
	return kJTrue;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

JBoolean
JPlotFitNonLinear::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	JBoolean ok	= itsVarList->GetNumericValue(index + 1, 1, value);
	return ok;
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

JBoolean
JPlotFitNonLinear::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	const JPlotDataBase* data = GetData();
	if (!data->HasXErrors() && !data->HasYErrors())
		{
		return kJFalse;
		}
	if (index > itsErrors->GetDimensionCount())
		{
		return kJFalse;
		}
	*value	= itsErrors->GetElement(index);
	return kJTrue;		
}
