/******************************************************************************
 JExprEditor.cpp

	Abstract base class to allow user interaction with a rendered JFunction.

	Only public and protected functions are required to call SaveStateForUndo(),
	and only if the action to be performed changes the function itself.
	Private functions must -not- call SaveStateForUndo(), because several
	manipulations may be required to perform one user command, and only the user
	command as a whole is undoable.  (Otherwise, the user may get confused.)

	Because of this convention, public functions that affect undo should only
	be a public interface to a private function.  The public function calls
	SaveStateForUndo() and then calls the private function.  The private function
	does all the work, but doesn't modify the undo information.  This allows other
	private functions to use the routine (private version) without modifying the
	undo information.

	Derived classes must implement the following routines:

		EIPRefresh
			Put an update event in the queue to redraw the function eventually.

		EIPRedraw
			Redraw the function immediately.

		EIPBoundsChanged
			Adjust the scroll bars to fit the size of the current function.

		EIPScrollToRect
			Scroll the function to make the given rectangle visible.  Return kJTrue
			if scrolling was necessary.

		EIPScrollForDrag
			Scroll the function to make the given point visible.  Return kJTrue
			if scrolling was necessary.

		EIPAdjustNeedTab
			If the OS prefers to use TAB for other purposes (like shifting focus),
			this routine should notify the OS whether or not it can use TAB.
			Since it is -very- much easier for the derived class to call this after
			menu selections and key presses, we require that it do it instead of
			us.  We do it only in those cases when the derived class would never
			hear of the change.

		EIPClipboardChanged
			Do whatever is appropriate to update the system clipboard
			after a Copy or Cut operation.

		EIPOwnsClipboard
			Return kJTrue if we should paste from our internal clipboard.

		EIPGetExternalClipboard
			Return kJTrue if there is something pasteable on the system clipboard.

	Derived classes can override the following routines:

		EIPExprChanged
			Override to set or clear whatever flags are needed to keep
			track of whether or not the current expression has been saved.

	BASE CLASS = JExprRenderer, virtual JBroadcaster

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JExprEditor.h>
#include <JExprNodeList.h>
#include <JExprRectList.h>
#include <JVariableList.h>
#include <JUserInputFunction.h>

#include <jParseFunction.h>
#include <jExprUIUtil.h>
#include <JFunctionWithVar.h>
#include <JUnaryFunction.h>
#include <JBinaryFunction.h>
#include <JNaryFunction.h>
#include <JSummation.h>
#include <JNegation.h>
#include <JProduct.h>
#include <JDivision.h>
#include <JExponent.h>
#include <JParallel.h>
#include <JConstantValue.h>

#include <JEPSPrinter.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <JString.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <jMath.h>
#include <ctype.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kEvalFnCmdStr        = "Evaluate function";
static const JCharacter* kEvalSelectionCmdStr = "Evaluate selection";

// JBroadcaster message types

const JCharacter* JExprEditor::kExprChanged = "ExprChanged::JExprEditor";

/******************************************************************************
 Constructor

	Derived class must call ClearFunction().

 ******************************************************************************/

JExprEditor::JExprEditor
	(
	const JVariableList*	varList,
	const JFontManager*		fontManager,
	JColormap*				colormap
	)
	:
	JExprRenderer(),
	itsFontManager(fontManager),
	itsColormap(colormap),
	itsTextColor(colormap->GetBlackColor()),
	itsSelectionColor(colormap->GetDefaultSelectionColor()),
	itsDefaultStyle(itsTextColor)
{
	itsVarList    = varList;
	itsFunction   = NULL;
	itsRectList   = NULL;
	itsSelection  = 0;
	itsActiveFlag = kJFalse;
	itsActiveUIF  = NULL;

	itsUndoFunction  = NULL;
	itsUndoSelection = 0;

	itsFunctionClip = NULL;

	itsPainter = NULL;

	itsDragType = kInvalidDrag;

	ListenTo(itsVarList);

	// We can't call ClearFunction() here because the _vtbl isn't fully
	// constructed.
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JExprEditor::~JExprEditor()
{
	assert( itsPainter == NULL );

	jdelete itsFunction;
	jdelete itsRectList;

	jdelete itsUndoFunction;
	jdelete itsFunctionClip;
}

/******************************************************************************
 SetFunction

	We take ownership of the function.

 ******************************************************************************/

void
JExprEditor::SetFunction
	(
	const JVariableList*	varList,
	JFunction*				f
	)
{
	assert( f != NULL );

	jdelete itsUndoFunction;
	itsUndoFunction  = NULL;
	itsUndoSelection = 0;

	if (itsVarList != varList)
		{
		StopListening(itsVarList);
		itsVarList = varList;
		ListenTo(itsVarList);
		}

	jdelete itsFunction;
	itsFunction  = f;
	itsActiveUIF = NULL;
	Render();
}

/******************************************************************************
 ClearFunction

 ******************************************************************************/

void
JExprEditor::ClearFunction()
{
	jdelete itsUndoFunction;
	itsUndoFunction  = NULL;
	itsUndoSelection = 0;

	JUserInputFunction* newUIF = PrivateClearFunction();
	ActivateUIF(newUIF);
}

/******************************************************************************
 PrivateClearFunction (private)

 ******************************************************************************/

JUserInputFunction*
JExprEditor::PrivateClearFunction()
{
	jdelete itsFunction;
	JUserInputFunction* newUIF =
		jnew JUserInputFunction(itsVarList, itsFontManager, itsColormap);
	assert( newUIF != NULL );
	itsFunction = newUIF;

	itsActiveUIF = NULL;
	Render();
	return newUIF;
}

/******************************************************************************
 ContainsUIF

	Returns kJTrue if we contain any JUserInputFunctions.

 ******************************************************************************/

JBoolean
JExprEditor::ContainsUIF()
	const
{
	const JSize rectCount = itsRectList->GetElementCount();
	for (JIndex i=1; i<=rectCount; i++)
		{
		const JFunction* f = itsRectList->GetFunction(i);
		if (f->GetType() == kJUserInputType)
			{
			return kJTrue;
			}
		}

	// falling through means there aren't any

	return kJFalse;
}

/******************************************************************************
 EIPDeactivate

	If the current UIF is invalid, we simply clear it.  This is because
	EndEditing() should already have been called.  If it wasn't, then the
	intent is to discard the function.

 ******************************************************************************/

void
JExprEditor::EIPDeactivate()
{
	if (itsActiveUIF != NULL)
		{
		itsActiveUIF->Clear();
		itsActiveUIF = NULL;
		Render();
		}
	else
		{
		ClearSelection();
		}

	itsActiveFlag = kJFalse;
}

/******************************************************************************
 EndEditing

	Parse the active JUserInputFunction and replace it.
	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JExprEditor::EndEditing()
{
	if (itsActiveUIF == NULL)
		{
		return kJTrue;
		}
	else if (itsActiveUIF->IsEmpty())
		{
		itsActiveUIF->Deactivate();
		itsActiveUIF = NULL;
		EIPRefresh();
		return kJTrue;
		}

	JFunction* newF = NULL;
	JUserInputFunction* newUIF = NULL;
	JBoolean needRender;
	if (itsActiveUIF->Parse(&newF, &newUIF, &needRender))
		{
		ReplaceFunction(itsActiveUIF, newF);
		itsActiveUIF = NULL;
		Render();
		return kJTrue;
		}
	else if (needRender)
		{
		JUserInputFunction* savedUIF = itsActiveUIF;
		itsActiveUIF = NULL;
		Render();
		itsActiveUIF = savedUIF;
		return kJFalse;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CancelEditing

	Clear the active JUserInputFunction and deactivate it.

 ******************************************************************************/

void
JExprEditor::CancelEditing()
{
	if (itsActiveUIF != NULL)
		{
		itsActiveUIF->Clear();
		itsActiveUIF->Deactivate();
		itsActiveUIF = NULL;
		Render();	// text of UIF may have changed
		}
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
JExprEditor::Undo()
{
	if (itsUndoFunction == NULL)
		{
		return;
		}

	(JGetUserNotification())->SetSilent(kJTrue);
	const JBoolean currFnOK = EndEditing();
	(JGetUserNotification())->SetSilent(kJFalse);

	if (currFnOK)
		{
		JFunction* savedF = itsFunction;
		itsFunction       = itsUndoFunction;
		itsUndoFunction   = savedF;

		JIndex savedSel  = itsSelection;
		itsSelection     = itsUndoSelection;
		itsUndoSelection = savedSel;

		// The issue isn't under the rug if we re-save itsSelection.
		// (Render() clears itsSelection)

		savedSel = itsSelection;
		Render();
		SetSelection(savedSel);
		}
	else
		{
		itsActiveUIF = NULL;	// kill editing

		itsFunction     = itsUndoFunction;
		itsUndoFunction = NULL;

		itsSelection     = itsUndoSelection;
		itsUndoSelection = 0;

		JIndex savedSel = itsSelection;
		Render();
		SetSelection(savedSel);
		}
}

/******************************************************************************
 SaveStateForUndo (private)

 ******************************************************************************/

void
JExprEditor::SaveStateForUndo()
{
	assert( itsFunction != NULL );

	jdelete itsUndoFunction;
	itsUndoFunction  = itsFunction->Copy();
	itsUndoSelection = itsSelection;

	Broadcast(ExprChanged());
}

/******************************************************************************
 EIPExprChanged (virtual protected)

	Derived classes can override to set or clear "expr saved" flags.

 ******************************************************************************/

void
JExprEditor::EIPExprChanged()
{
}

/******************************************************************************
 Cut

 ******************************************************************************/

void
JExprEditor::Cut()
{
	JFunction* f;
	if (Cut(&f))
		{
		jdelete itsFunctionClip;
		itsFunctionClip = f;
		EIPClipboardChanged();
		}
}

/******************************************************************************
 Cut

	Returns kJTrue if there was anything to cut.
	If function returns kJFalse, *f is not modified.

 ******************************************************************************/

JBoolean
JExprEditor::Cut
	(
	JFunction** f
	)
{
	if (Copy(f))
		{
		DeleteSelection();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Copy

 ******************************************************************************/

void
JExprEditor::Copy()
{
	JFunction* f;
	if (Copy(&f))
		{
		jdelete itsFunctionClip;
		itsFunctionClip = f;
		EIPClipboardChanged();
		}
}

/******************************************************************************
 Copy

	Returns kJTrue if there was anything to copy.
	If function returns kJFalse, *f is not modified.

 ******************************************************************************/

JBoolean
JExprEditor::Copy
	(
	JFunction** f
	)
	const
{
	const JFunction* selF;
	if (GetConstSelectedFunction(&selF))
		{
		*f = selF->Copy();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Paste

 ******************************************************************************/

void
JExprEditor::Paste()
{
	JUserNotification* un = JGetUserNotification();

	JString text;
	PasteResult result = kPasteOK;
	if (EIPOwnsClipboard())
		{
		result = Paste(*itsFunctionClip);
		}
	else if (EIPGetExternalClipboard(&text))
		{
		un->SetSilent(kJTrue);
		result = Paste(text);
		un->SetSilent(kJFalse);

		if (result == kParseError)
			{
			JString errorStr = "Unable to parse \"";
			errorStr += text;
			errorStr += "\"";
			un->ReportError(errorStr);
			}
		}

	if (result == kNowhereToPaste)
		{
		un->ReportError("Please select a place to paste first.");
		}
}

/******************************************************************************
 Paste

	Returns kJTrue if expr parsed successfully.

	We don't call Paste(f) because that would require an extra copy operation,
	which can be expensive for JFuntions.

 ******************************************************************************/

JExprEditor::PasteResult
JExprEditor::Paste
	(
	const JCharacter* expr
	)
{
	JFunction* selF;
	if (GetSelectedFunction(&selF))
		{
		JFunction* newF;
		if (JParseFunction(expr, itsVarList, &newF, kJTrue))
			{
			SaveStateForUndo();
			ReplaceFunction(selF, newF);
			Render();
			SelectFunction(newF);
			return kPasteOK;
			}
		else
			{
			return kParseError;
			}
		}
	else if (itsActiveUIF != NULL && itsActiveUIF->IsEmpty())
		{
		JFunction* newF;
		if (JParseFunction(expr, itsVarList, &newF, kJTrue))
			{
			SaveStateForUndo();
			ReplaceFunction(itsActiveUIF, newF);
			itsActiveUIF = NULL;
			Render();
			SelectFunction(newF);
			return kPasteOK;
			}
		else
			{
			return kParseError;
			}
		}
	else
		{
		return kNowhereToPaste;
		}
}

/******************************************************************************
 Paste

 ******************************************************************************/

JExprEditor::PasteResult
JExprEditor::Paste
	(
	const JFunction& f
	)
{
	JFunction* selF;
	if (GetSelectedFunction(&selF))
		{
		JFunction* newF = f.Copy();
		SaveStateForUndo();
		ReplaceFunction(selF, newF);
		Render();
		SelectFunction(newF);
		return kPasteOK;
		}
	else if (itsActiveUIF != NULL && itsActiveUIF->IsEmpty())
		{
		JFunction* newF = f.Copy();
		SaveStateForUndo();
		ReplaceFunction(itsActiveUIF, newF);
		itsActiveUIF = NULL;
		Render();
		SelectFunction(newF);
		return kPasteOK;
		}
	else
		{
		return kNowhereToPaste;
		}
}

/******************************************************************************
 DeleteSelection

 ******************************************************************************/

void
JExprEditor::DeleteSelection()
{
	if (HasSelection())
		{
		SaveStateForUndo();
		DeleteFunction( itsRectList->GetFunction(itsSelection) );
		Render();
		}
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

void
JExprEditor::SelectAll()
{
	SelectFunction(itsFunction);
}

/******************************************************************************
 EvaluateSelection

 ******************************************************************************/

JBoolean
JExprEditor::EvaluateSelection
	(
	JFloat* value
	)
	const
{
	const JFunction* f;
	if (GetConstSelectedFunction(&f))
		{
		return f->Evaluate(value);
		}
	else
		{
		return itsFunction->Evaluate(value);
		}
}

/******************************************************************************
 NegateSelection

 ******************************************************************************/

void
JExprEditor::NegateSelection()
{
	JFunction* f = itsActiveUIF;
	if (f != NULL || GetSelectedFunction(&f))
		{
		SaveStateForUndo();
		JFunction* arg = f->Copy();
		JNegation* neg = jnew JNegation(arg);
		assert( neg != NULL );
		itsActiveUIF = NULL;
		ReplaceFunction(f, neg);
		Render();
		SelectFunction(arg);
		}
}

/******************************************************************************
 ApplyFunctionToSelection

 ******************************************************************************/

void
JExprEditor::ApplyFunctionToSelection
	(
	const JCharacter* fnName
	)
{
	JFunction* f = itsActiveUIF;
	if (f != NULL || GetSelectedFunction(&f))
		{
		JFunction* newF;
		JFunction* newArg;
		JUserInputFunction* newUIF;
		if (JApplyFunction(fnName, itsVarList, *f, itsFontManager, itsColormap,
						   &newF, &newArg, &newUIF))
			{
			SaveStateForUndo();
			itsActiveUIF = NULL;
			ReplaceFunction(f, newF);
			Render();
			if (newUIF != NULL)
				{
				ActivateUIF(newUIF);
				}
			else
				{
				SelectFunction(newArg);
				}
			}
		}
}

/******************************************************************************
 AddArgument

 ******************************************************************************/

void
JExprEditor::AddArgument()
{
	JFunction* f;
	if (GetSelectedFunction(&f))
		{
		JNaryFunction* naryF = f->CastToJNaryFunction();
		if (naryF != NULL)
			{
			SaveStateForUndo();
			JUserInputFunction* uif =
				jnew JUserInputFunction(itsVarList, itsFontManager, itsColormap);
			assert( uif != NULL );
			naryF->AppendArg(uif);
			Render();
			ActivateUIF(uif);
			}
		}
}

/******************************************************************************
 MoveArgument

	delta specifies how many slots to shift the selected argument by.

	delta<0 => shift left
	delta>0 => shift right

 ******************************************************************************/

void
JExprEditor::MoveArgument
	(
	const JInteger delta
	)
{
	JFunction* f;
	JFunction* parentF;
	if (delta != 0 && GetNegAdjSelFunction(&f, &parentF))
		{
		JNaryFunction* naryParentF = parentF->CastToJNaryFunction();
		if (naryParentF != NULL)
			{
			JIndex argIndex;
			const JBoolean found = naryParentF->FindArg(f, &argIndex);
			assert( found );
			JIndex newIndex = argIndex + delta;
			if (naryParentF->ArgIndexValid(newIndex))
				{
				SaveStateForUndo();
				naryParentF->MoveArgToIndex(argIndex, newIndex);
				itsActiveUIF = NULL;
				Render();
				SelectFunction(f);
				}
			}
		}
}

/******************************************************************************
 GroupArguments

	delta specifies how many arguments to group together with the selected one.

	delta<0 => group selected arg with -delta-1 args to the left
	delta>0 => group selected arg with  delta-1 args to the right

 ******************************************************************************/

void
JExprEditor::GroupArguments
	(
	const JInteger delta
	)
{
	JFunction* f;
	JFunction* parentF;
	if (delta != 0 && GetNegAdjSelFunction(&f, &parentF))
		{
		JNaryOperator* naryParentF = parentF->CastToJNaryOperator();
		if (naryParentF != NULL)
			{
			JIndex argIndex;
			const JBoolean found = naryParentF->FindArg(f, &argIndex);
			assert( found );
			const JIndex firstArg = JMin(argIndex, argIndex+delta);
			const JIndex lastArg  = JMax(argIndex, argIndex+delta);

			const JSize parentArgCount = naryParentF->GetArgCount();

			const JBoolean sameType =
				JI2B( parentF->GetType() == f->GetType() );
			const JBoolean argsInRange =
				JI2B( 1 <= firstArg && lastArg <= parentArgCount );
			const JBoolean groupAll =
				JI2B( lastArg - firstArg + 1 == parentArgCount );

			JUnaryFunction* neg = f->CastToJUnaryFunction();
			const JBoolean extendNegation =
				JI2B( parentF->GetType() == kJSummationType &&
					  neg != NULL && neg->GetType() == kJNegationType &&
					  (neg->GetArg())->GetType() == kJSummationType );

			// handle special case w+x-(y+z)
			// (if gobbles last argument, remove parentF entirely)

			if (extendNegation && argsInRange)
				{
				SaveStateForUndo();
				JNaryOperator* naryF = (neg->GetArg())->CastToJNaryOperator();
				assert( naryF != NULL );
				if (delta > 0)
					{
					for (JIndex i=1; i <= (JSize) delta; i++)
						{
						naryF->AppendArg(Negate(*(naryParentF->GetArg(argIndex+1))));
						naryParentF->DeleteArg(argIndex+1);
						}
					}
				else	// from above, know that delta != 0
					{
					for (JIndex i=1; i <= (JSize) -delta; i++)
						{
						naryF->PrependArg(Negate(*(naryParentF->GetArg(argIndex-i))));
						naryParentF->DeleteArg(argIndex-i);
						}
					}
				itsActiveUIF = NULL;
				if (groupAll)
					{
					f = f->Copy();
					ReplaceFunction(parentF, f);
					}
				Render();
				SelectFunction(f);
				}

			// create a new JNaryOperator to contain the group

			else if (!sameType && argsInRange && !groupAll)
				{
				JIndex i;
				SaveStateForUndo();

				// Copy the parent function and delete the args that
				// will not be in the group.

				JFunction* newF      = parentF->Copy();
				JNaryOperator* group = newF->CastToJNaryOperator();
				assert( group != NULL );
				while (group->GetArgCount() > lastArg)
					{
					group->DeleteArg(lastArg+1);
					}
				for (i=1; i<firstArg; i++)
					{
					group->DeleteArg(1);
					}

				// Replace the original args from the parent function
				// with the new group.

				for (i=1; i<=lastArg-firstArg+1; i++)
					{
					naryParentF->DeleteArg(firstArg);
					}
				naryParentF->InsertArg(firstArg, group);

				// show the result

				itsActiveUIF = NULL;
				Render();
				SelectFunction(group);
				}

			// add the arguments to the existing group

			else if (sameType && argsInRange && !groupAll)
				{
				SaveStateForUndo();
				JNaryOperator* naryF = f->CastToJNaryOperator();
				assert( naryF != NULL );
				if (delta > 0)
					{
					for (JIndex i=1; i <= (JSize) delta; i++)
						{
						naryF->AppendArg((naryParentF->GetArg(argIndex+1))->Copy());
						naryParentF->DeleteArg(argIndex+1);
						}
					}
				else	// from above, know that delta != 0
					{
					for (JIndex i=1; i <= (JSize) -delta; i++)
						{
						naryF->PrependArg((naryParentF->GetArg(argIndex-i))->Copy());
						naryParentF->DeleteArg(argIndex-i);
						}
					}
				itsActiveUIF = NULL;
				Render();
				SelectFunction(f);
				}

			// group all arguments => just remove existing group

			else if (sameType && argsInRange && groupAll)
				{
				UngroupArguments();
				}
			}
		}
}

/******************************************************************************
 UngroupArguments

	Absorb the grouped arguments into the parent function.

 ******************************************************************************/

void
JExprEditor::UngroupArguments()
{
	JFunction* f;
	JFunction* parentF;
	if (GetNegAdjSelFunction(&f, &parentF))
		{
		JNaryOperator* naryF       = f->CastToJNaryOperator();
		JNaryOperator* naryParentF = parentF->CastToJNaryOperator();
		if (naryF != NULL && naryParentF != NULL &&
			naryF->GetType() == naryParentF->GetType())
			{
			SaveStateForUndo();

			JIndex argIndex;
			const JBoolean found = naryParentF->FindArg(naryF, &argIndex);
			assert( found );

			const JSize fArgCount = naryF->GetArgCount();
			for (JIndex i=1; i<=fArgCount; i++)
				{
				naryParentF->InsertArg(argIndex+i, (naryF->GetArg(i))->Copy());
				}
			naryParentF->DeleteArg(argIndex);

			itsActiveUIF = NULL;
			Render();
			SelectFunction(naryParentF);
			}

		// handle special case w+x-(y+z)

		else if (naryParentF != NULL &&
				 naryParentF->GetType() == kJSummationType &&
				 f->GetType()           == kJNegationType)
			{
			const JUnaryFunction* neg = f->CastToJUnaryFunction();
			assert( neg != NULL );
			const JNaryOperator* naryF = (neg->GetArg())->CastToJNaryOperator();
			if (naryF != NULL && naryF->GetType() == kJSummationType)
				{
				SaveStateForUndo();

				JIndex argIndex;
				const JBoolean found = naryParentF->FindArg(f, &argIndex);
				assert( found );

				const JSize fArgCount = naryF->GetArgCount();
				for (JIndex i=1; i<=fArgCount; i++)
					{
					naryParentF->InsertArg(argIndex+i, Negate(*(naryF->GetArg(i))));
					}
				naryParentF->DeleteArg(argIndex);

				itsActiveUIF = NULL;
				Render();
				SelectFunction(naryParentF);
				}
			}
		}
}

/******************************************************************************
 Negate (private)

	Returns a copy of the negation of the given function.  If the function is
	already a negation, this is simply removed.

 ******************************************************************************/

JFunction*
JExprEditor::Negate
	(
	const JFunction& f
	)
	const
{
	if (f.GetType() == kJNegationType)
		{
		const JUnaryFunction* neg = f.CastToJUnaryFunction();
		assert( neg != NULL );
		return (neg->GetArg())->Copy();
		}
	else
		{
		JFunction* neg = jnew JNegation(f.Copy());
		assert( neg != NULL );
		return neg;
		}
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

JBoolean
JExprEditor::HasSelection()
	const
{
	return JConvertToBoolean(
			itsRectList != NULL && itsRectList->SelectionValid(itsSelection) );
}

/******************************************************************************
 GetSelection (protected)

 ******************************************************************************/

JBoolean
JExprEditor::GetSelection
	(
	JIndex* selection
	)
	const
{
	*selection = itsSelection;
	return HasSelection();
}

/******************************************************************************
 GetSelectionRect (protected)

 ******************************************************************************/

JBoolean
JExprEditor::GetSelectionRect
	(
	JRect* selRect
	)
	const
{
	if (HasSelection())
		{
		*selRect = itsRectList->GetRect(itsSelection);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetSelectedFunction

 ******************************************************************************/

// protected

JBoolean
JExprEditor::GetSelectedFunction
	(
	JFunction** f
	)
{
	if (HasSelection())
		{
		*f = itsRectList->GetFunction(itsSelection);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

// public

JBoolean
JExprEditor::GetConstSelectedFunction	// to avoid "ambiguous reference" errors
	(
	const JFunction** f
	)
	const
{
	if (HasSelection())
		{
		*f = itsRectList->GetFunction(itsSelection);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetNegAdjSelFunction (private)

	If possible, returns the selected function and its parent.
	If the selected function is y in the case of "x-y", then it returns
	"-y" as the function.  This is the entire reason for the function.
	It returns the "negation adjusted" selected function.

	This treats an active UIF as if it were selected.

	Returns kJFalse if the selected function has no parent.

 ******************************************************************************/

JBoolean
JExprEditor::GetNegAdjSelFunction
	(
	JFunction** selF,
	JFunction** parentF
	)
	const
{
	*selF = itsActiveUIF;
	if ((*selF != NULL ||
		 GetConstSelectedFunction(const_cast<const JFunction**>(selF))) &&
		*selF != itsFunction)
		{
		JExprNodeList nodeList(itsFunction);
		*parentF = GetParentFunction(*selF, nodeList);

		JFunction* grandparentF = NULL;
		if (*parentF != itsFunction)
			{
			grandparentF = GetParentFunction(*parentF, nodeList);
			}

		if ((**parentF).GetType() == kJNegationType && grandparentF != NULL &&
			grandparentF->GetType() == kJSummationType)
			{
			*selF    = *parentF;
			*parentF = grandparentF;
			}

		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetSelection (protected)

 ******************************************************************************/

void
JExprEditor::SetSelection
	(
	const JIndex index
	)
{
	if (itsRectList != NULL &&
		index != itsSelection && itsRectList->SelectionValid(index))
		{
		itsSelection = index;
		if (!EIPScrollToRect(itsRectList->GetRect(itsSelection)))
			{
			EIPRedraw();
			}
		}
	else if (itsRectList != NULL &&
			 index == 0 && itsSelection != 0)
		{
		itsSelection = 0;
		EIPRedraw();
		}
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

void
JExprEditor::ClearSelection()
{
	if (itsRectList != NULL &&
		itsRectList->SelectionValid(itsSelection))
		{
		itsSelection = 0;
		EIPRedraw();
		}
}

/******************************************************************************
 SelectFunction

	If we can't select the specified function, we try each of its arguments.
	We can always select terminal nodes, so we won't get stuck.

	If f is a JUIF, we activate it instead.

	We need this mainly because JSummation hides JNegations.

 ******************************************************************************/

void
JExprEditor::SelectFunction
	(
	const JFunction* f
	)
{
	const JBoolean wasItsFunction = JI2B( f == itsFunction );
	if (itsRectList != NULL && EndEditing())
		{
		if (wasItsFunction)		// in case itsFunction was single, active UIF
			{
			f = itsFunction;
			}

		JIndex fIndex;
		JBoolean found = itsRectList->FindFunction(f, &fIndex);
		while (!found)
			{
			const JFunctionWithArgs* fwa = f->CastToJFunctionWithArgs();
			assert( fwa != NULL && fwa->GetArgCount() > 0 );
			f     = fwa->GetArg(1);
			found = itsRectList->FindFunction(f, &fIndex);
			}

		if (f->GetType() == kJUserInputType)
			{
			const JUserInputFunction* uif = dynamic_cast<const JUserInputFunction*>(f);
			assert( uif != NULL );
			ActivateUIF(const_cast<JUserInputFunction*>(uif));
			}
		else
			{
			SetSelection(fIndex);
			}
		}
}

/******************************************************************************
 Set font

	Set the current font of the active UIF.

 ******************************************************************************/

void
JExprEditor::SetNormalFont()
{
	if (itsActiveUIF != NULL)
		{
		itsActiveUIF->SetCurrentFontName(JGetDefaultFontName());
		JUserInputFunction* savedUIF = itsActiveUIF;
		itsActiveUIF = NULL;
		Render();
		itsActiveUIF = savedUIF;
		}
}

void
JExprEditor::SetGreekFont()
{
	if (itsActiveUIF != NULL)
		{
		itsActiveUIF->SetCurrentFontName(JGetGreekFontName());
		JUserInputFunction* savedUIF = itsActiveUIF;
		itsActiveUIF = NULL;
		Render();
		itsActiveUIF = savedUIF;
		}
}

/******************************************************************************
 GetCmdStatus (protected)

	Returns an array indicating which commands can be performed in the
	current state.

	evalStr can be NULL.

 ******************************************************************************/

JArray<JBoolean>
JExprEditor::GetCmdStatus
	(
	const JCharacter** evalStr
	)
	const
{
	JArray<JBoolean> flags(kCmdCount);
	for (JIndex i=1; i<=kCmdCount; i++)
		{
		flags.AppendElement(kJFalse);
		}

	if (itsFunction == NULL || itsRectList == NULL || !itsActiveFlag)
		{
		return flags;
		}

	flags.SetElement(kEvaluateSelCmd, kJTrue);
	flags.SetElement(kSelectAllCmd, kJTrue);
	flags.SetElement(kPrintEPSCmd, kJTrue);

	const JBoolean hasSelection = itsRectList->SelectionValid(itsSelection);

	if (itsUndoFunction != NULL)
		{
		flags.SetElement(kUndoCmd, kJTrue);
		}
	if (hasSelection || (itsActiveUIF != NULL && itsActiveUIF->IsEmpty()))
		{
		flags.SetElement(kPasteCmd, kJTrue);
		}

	if (evalStr != NULL)
		{
		*evalStr = (hasSelection ? kEvalSelectionCmdStr : kEvalFnCmdStr);
		}

	if (hasSelection)
		{
		flags.SetElement(kCutCmd, kJTrue);
		flags.SetElement(kCopyCmd, kJTrue);
		flags.SetElement(kDeleteSelCmd, kJTrue);

		flags.SetElement(kNegateSelCmd, kJTrue);
		flags.SetElement(kApplyFnToSelCmd, kJTrue);

		JFunction* selF = itsRectList->GetFunction(itsSelection);
		if (selF->CastToJNaryFunction() != NULL)
			{
			flags.SetElement(kAddArgCmd, kJTrue);
			}
		}

	if (itsActiveUIF != NULL)
		{
		flags.SetElement(kNegateSelCmd, kJTrue);
		flags.SetElement(kApplyFnToSelCmd, kJTrue);

		const JFont font = itsActiveUIF->GetCurrentFont();
		if (strcmp(font.GetName(), JGetDefaultFontName()) == 0)
			{
			flags.SetElement(kSetGreekFontCmd, kJTrue);
			}
		else
			{
			flags.SetElement(kSetNormalFontCmd, kJTrue);
			}
		}

	JFunction *selF, *parentF;
	if (GetNegAdjSelFunction(&selF, &parentF))
		{
		JNaryFunction* naryParentF = parentF->CastToJNaryFunction();
		if (naryParentF != NULL)
			{
			const JSize parentArgCount = naryParentF->GetArgCount();
			JIndex argIndex;
			const JBoolean found = naryParentF->FindArg(selF, &argIndex);
			assert( found );
			if (argIndex > 1)
				{
				flags.SetElement(kMoveArgLeftCmd, kJTrue);
				}
			if (argIndex < parentArgCount)
				{
				flags.SetElement(kMoveArgRightCmd, kJTrue);
				}

			if (naryParentF->CastToJNaryOperator() != NULL)
				{
				if (argIndex > 1)
					{
					flags.SetElement(kGroupLeftCmd, kJTrue);
					}
				if (argIndex < parentArgCount)
					{
					flags.SetElement(kGroupRightCmd, kJTrue);
					}
				if (selF->GetType() == naryParentF->GetType())
					{
					flags.SetElement(kUngroupCmd, kJTrue);
					}
				else if (naryParentF->GetType() == kJSummationType &&
						 selF->GetType()        == kJNegationType)
					{
					const JUnaryFunction* neg = selF->CastToJUnaryFunction();
					assert( neg != NULL );
					if ((neg->GetArg())->GetType() == kJSummationType)
						{
						flags.SetElement(kUngroupCmd, kJTrue);
						}
					}
				}
			}
		}

	return flags;
}

/******************************************************************************
 Print

	EPS

 ******************************************************************************/

void
JExprEditor::Print
	(
	JEPSPrinter& p
	)
{
	const JRect bounds = GetPrintBounds();

	if (p.WantsPreview())
		{
		JPainter& p1 = p.GetPreviewPainter(bounds);
		EIPDraw(p1);
		}

	if (p.OpenDocument(bounds))
		{
		EIPDraw(p);
		p.CloseDocument();
		}
}

/******************************************************************************
 GetPrintBounds

 ******************************************************************************/

JRect
JExprEditor::GetPrintBounds()
	const
{
	return itsRectList->GetBoundsRect();
}

/******************************************************************************
 DrawForPrint

	Anywhere on the page

	Not called Print() because it would conflict with other prototypes.

 ******************************************************************************/

void
JExprEditor::DrawForPrint
	(
	JPainter&		p,
	const JPoint&	topLeft
	)
{
	const JRect bounds = GetPrintBounds();

	p.ShiftOrigin(topLeft);
	EIPDraw(p);
	p.ShiftOrigin(-topLeft);
}

/******************************************************************************
 EIPDraw (protected)

 ******************************************************************************/

void
JExprEditor::EIPDraw
	(
	JPainter& p
	)
{
	assert( itsPainter == NULL );
	itsPainter = &p;

	// hilight selection

	JRect selRect;
	if (GetSelectionRect(&selRect))
		{
		p.SetPenColor(itsSelectionColor);
		p.SetFilling(kJTrue);
		p.Rect(selRect);
		p.SetFilling(kJFalse);
		p.SetPenColor(itsTextColor);
		}

	// draw the function

	itsFunction->Render(*this, *itsRectList);

	// clean up

	itsPainter = NULL;
}

/******************************************************************************
 EIPHandleMouseDown (protected)

 ******************************************************************************/

void
JExprEditor::EIPHandleMouseDown
	(
	const JPoint&	currPt,
	const JBoolean	extend
	)
{
	itsDragType = kInvalidDrag;
	if (!itsActiveFlag)
		{
		return;
		}

	// If the user clicked on the active UIF, let it handle the click.

	if (itsRectList != NULL && MouseOnActiveUIF(currPt))
		{
		itsDragType = kSendToUIF;
		const JBoolean redraw =
			itsActiveUIF->HandleMouseDown(currPt, extend, *itsRectList, *this);
		if (redraw)
			{
			EIPRedraw();
			}
		}

	// The click is for us.  We let the user select something.

	else if (itsRectList != NULL)
		{
		itsDragType = kSelectExpr;
		if (itsActiveUIF != NULL && !EndEditing())
			{
			itsDragType = kInvalidDrag;
			return;
			}

		itsPrevSelectedFunction = NULL;
		const JBoolean hasSelection = itsRectList->SelectionValid(itsSelection);
		if (hasSelection)
			{
			itsPrevSelectedFunction = itsRectList->GetFunction(itsSelection);
			}

		if (extend && hasSelection)
			{
			const JRect origSelRect = itsRectList->GetRect(itsSelection);
			itsStartPt.x = origSelRect.xcenter();
			itsStartPt.y = origSelRect.ycenter();
			}
		else
			{
			itsStartPt = currPt;
			}

		const JIndex newSelection = itsRectList->GetSelection(itsStartPt, currPt);
		SetSelection(newSelection);
		}
}

/******************************************************************************
 EIPHandleMouseDrag (protected)

 ******************************************************************************/

void
JExprEditor::EIPHandleMouseDrag
	(
	const JPoint& currPt
	)
{
	if (itsRectList != NULL && itsDragType == kSelectExpr)
		{
		EIPScrollForDrag(currPt);

		const JIndex newSelection = itsRectList->GetSelection(itsStartPt, currPt);
		if (newSelection != itsSelection)
			{
			itsSelection = newSelection;
			EIPRedraw();
			}
		}
	else if (itsRectList != NULL && itsDragType == kSendToUIF)
		{
		const JBoolean redraw =
			itsActiveUIF->HandleMouseDrag(currPt, *itsRectList, *this);
		if (redraw)
			{
			EIPRedraw();
			}
		}
}

/******************************************************************************
 EIPHandleMouseUp (protected)

	Activate a JUserInputFunction if only it was selected.
	Allow editing of a number if it was clicked on a second time.

 ******************************************************************************/

void
JExprEditor::EIPHandleMouseUp()
{
	if (itsRectList != NULL && itsDragType == kSelectExpr &&
		itsRectList->SelectionValid(itsSelection))
		{
		assert( itsActiveUIF == NULL );

		JFunction* selectedF = itsRectList->GetFunction(itsSelection);
		const JFunctionType selectedFType = selectedF->GetType();
		if (selectedFType == kJUserInputType)
			{
			JUserInputFunction* uif = dynamic_cast<JUserInputFunction*>(selectedF);
			assert( uif != NULL );
			ActivateUIF(uif);
			assert( itsActiveUIF != NULL );
			itsActiveUIF->HandleMouseDown(itsStartPt, kJFalse, *itsRectList, *this);
			itsActiveUIF->HandleMouseUp();
			EIPRefresh();
			}
		else if ((selectedFType == kJConstantValueType ||
				  selectedFType == kJVariableValueType) &&
				 selectedF == itsPrevSelectedFunction)
			{
			SaveStateForUndo();
			JString s;
			if (selectedFType == kJConstantValueType)
				{
				JConstantValue* constVal = dynamic_cast<JConstantValue*>(selectedF);
				assert( constVal != NULL );
				s = constVal->GetValue();
				}
			else
				{
				assert( selectedFType == kJVariableValueType );
				JFunctionWithVar* varVal = selectedF->CastToJFunctionWithVar();
				assert( varVal != NULL );
				s = itsVarList->GetVariableName(varVal->GetVariableIndex());
				}
			JUserInputFunction* newUIF =
				jnew JUserInputFunction(itsVarList, itsFontManager, itsColormap, s);
			assert( newUIF != NULL );
			ReplaceFunction(selectedF, newUIF);
			Render();
			ActivateUIF(newUIF);
			assert( itsActiveUIF != NULL );
			itsActiveUIF->HandleMouseDown(itsStartPt, kJFalse, *itsRectList, *this);
			itsActiveUIF->HandleMouseUp();
			EIPRefresh();
			}
		}
	else if (itsDragType == kSendToUIF)
		{
		const JBoolean redraw = itsActiveUIF->HandleMouseUp();
		if (redraw)
			{
			EIPRefresh();
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 MouseOnActiveUIF (protected)

 ******************************************************************************/

JBoolean
JExprEditor::MouseOnActiveUIF
	(
	const JPoint& pt
	)
	const
{
	JIndex uifIndex;
	return JConvertToBoolean(
				itsRectList != NULL && itsActiveUIF != NULL &&
				itsRectList->FindFunction(itsActiveUIF, &uifIndex) &&
				itsRectList->GetSelection(pt, pt) == uifIndex);
}

/******************************************************************************
 EIPHandleKeyPress (protected)

 ******************************************************************************/

void
JExprEditor::EIPHandleKeyPress
	(
	const JCharacter key
	)
{
	if (itsFunction == NULL || itsRectList == NULL || !itsActiveFlag)
		{
		return;
		}
	else if (key == '\t')
		{
		ActivateNextUIF();
		return;
		}

	const JBoolean selectionValid = itsRectList->SelectionValid(itsSelection);
	if (!selectionValid && itsActiveUIF == NULL)
		{
		return;
		}
	assert( ( selectionValid && itsActiveUIF == NULL) ||
			(!selectionValid && itsActiveUIF != NULL) );

	const JBoolean isOperatorKey =
		JConvertToBoolean(
			key == '+' || (key == '-' && (selectionValid || !itsActiveUIF->IsEmpty())) ||
			key == '*' || key == '/' || key == '^' || key == '|' ||
			(key == ',' && CanApplyCommaOperator())
			);

	if (selectionValid && (key == ' ' || key == ')'))
		{
		SetSelection(itsRectList->GetParent(itsSelection));
		}

	else if (selectionValid && key == kJDeleteKey)
		{
		SaveStateForUndo();

		// replace selection with JUserInputFunction

		JUserInputFunction* newUIF =
			jnew JUserInputFunction(itsVarList, itsFontManager, itsColormap);
		assert( newUIF != NULL );
		JFunction* selectedF = itsRectList->GetFunction(itsSelection);
		ReplaceFunction(selectedF, newUIF);
		Render();
		ActivateUIF(newUIF);
		}

	else if (selectionValid && JIsPrint(key))
		{
		SaveStateForUndo();

		JFunction* selectedF = itsRectList->GetFunction(itsSelection);
		if (isOperatorKey)
			{
			ApplyOperatorKey(key, selectedF);
			}
		else
			{
			// replace selection with JUserInputFunction

			JUserInputFunction* newUIF =
				jnew JUserInputFunction(itsVarList, itsFontManager, itsColormap);
			assert( newUIF != NULL );
			newUIF->Activate();
			JBoolean needParse, needRender;
			newUIF->HandleKeyPress(key, &needParse, &needRender);
			assert( !needParse );	// it was just created
			newUIF->Deactivate();

			ReplaceFunction(selectedF, newUIF);
			Render();
			ActivateUIF(newUIF);
			}
		}

	else if (itsActiveUIF != NULL && key == kJEscapeKey)
		{
		if (!itsActiveUIF->IsEmpty())
			{
			itsActiveUIF->Clear();
			JUserInputFunction* savedUIF = itsActiveUIF;
			itsActiveUIF = NULL;
			Render();
			itsActiveUIF = savedUIF;
			}
		}

	else if (itsActiveUIF != NULL &&
			 (isOperatorKey ||
			  key == '\n' || key == '\r' || key == ' ' || key == ')'))
		{
		JFunction* arg1 = NULL;
		JUserInputFunction* newUIF = NULL;
		JBoolean needRender;
		if (itsActiveUIF->Parse(&arg1, &newUIF, &needRender))
			{
			assert( newUIF == NULL );
			ReplaceFunction(itsActiveUIF, arg1);
			itsActiveUIF = NULL;
			if (isOperatorKey)
				{
				SaveStateForUndo();
				ApplyOperatorKey(key, arg1);
				}
			else
				{
				assert( key == '\n' || key == '\r' || key == ' ' || key == ')' );

				// select the new function

				Render();
				SelectFunction(arg1);
				}
			}
		else if (needRender)
			{
			JUserInputFunction* savedUIF = itsActiveUIF;
			itsActiveUIF = NULL;
			Render();
			itsActiveUIF = savedUIF;
			}
		}

	else if (itsActiveUIF != NULL && itsActiveUIF != itsFunction &&
			 itsActiveUIF->IsEmpty() && key == kJDeleteKey)
		{
		SaveStateForUndo();

		JFunction* fToSelect = DeleteFunction(itsActiveUIF);
		itsActiveUIF = NULL;
		Render();
		SelectFunction(fToSelect);
		}

	else if (itsActiveUIF != NULL)
		{
		SendKeyToActiveUIF(key);
		}
}

/******************************************************************************
 ApplyOperatorKey (private)

	Given the operator key, modify the target function appropriately.

 ******************************************************************************/

void
JExprEditor::ApplyOperatorKey
	(
	const JCharacter	key,
	JFunction*			targetF
	)
{
	const JFunctionType targetType = targetF->GetType();

	JFunction* parentF = NULL;
	JFunctionType parentType = kJVariableValueType;
	if (targetF != itsFunction)
		{
		parentF    = GetParentFunction(targetF);
		parentType = parentF->GetType();
		}

	JUserInputFunction* newUIF =
		jnew JUserInputFunction(itsVarList, itsFontManager, itsColormap);
	assert( newUIF != NULL );

	JFunction* newArg = newUIF;
	if (key == '-')
		{
		newArg = jnew JNegation(newUIF);
		assert( newArg != NULL );
		}

	JFunction* newF = NULL;

	if (((key == '+' || key == '-') && targetType == kJSummationType) ||
		(key == '*' && targetType == kJProductType) ||
		(key == '|' && targetType == kJParallelType))
		{
		JNaryOperator* naryOp = targetF->CastToJNaryOperator();
		assert( naryOp != NULL );
		naryOp->SetArg(naryOp->GetArgCount()+1, newArg);
		}
	else if (parentF != NULL &&
			(((key == '+' || key == '-') && parentType == kJSummationType) ||
			 (key == '*' && parentType == kJProductType) ||
			 (key == '|' && parentType == kJParallelType)))
		{
		JNaryOperator* naryOp = parentF->CastToJNaryOperator();
		assert( naryOp != NULL );
		JIndex selFIndex;
		const JBoolean found = naryOp->FindArg(targetF, &selFIndex);
		assert( found );
		naryOp->InsertArg(selFIndex+1, newArg);
		}
	else if (key == '+' || key == '-' || key == '*' || key == '|')
		{
		JNaryOperator* newOp = NULL;
		if (key == '+' || key == '-')
			{
			newOp = jnew JSummation;
			}
		else if (key == '*')
			{
			newOp = jnew JProduct;
			}
		else
			{
			assert( key == '|' );
			newOp = jnew JParallel;
			}
		assert( newOp != NULL );
		newOp->SetArg(1, targetF->Copy());
		newOp->SetArg(2, newArg);
		newF = newOp;
		}

	else if (key == '/')
		{
		newF = jnew JDivision(targetF->Copy(), newArg);
		assert( newF != NULL );
		}

	else if (key == '^')
		{
		newF = jnew JExponent(targetF->Copy(), newArg);
		assert( newF != NULL );
		}

	else if (key == ',')
		{
		JNaryFunction* commaTargetF;
		JIndex newArgIndex;
		const JBoolean found = GetCommaTarget(targetF, &commaTargetF, &newArgIndex);
		assert( found );
		commaTargetF->InsertArg(newArgIndex, newArg);
		}

	else
		{
		assert( 0 );	// this is an error and should never happen
		}

	if (newF != NULL)
		{
		ReplaceFunction(targetF, newF);
		}
	Render();
	ActivateUIF(newUIF);
}

/******************************************************************************
 CanApplyCommaOperator (private)

	Returns kJTrue if it is possible to apply the comma operator to the
	current selection or active UIF.

 ******************************************************************************/

JBoolean
JExprEditor::CanApplyCommaOperator()
{
	JFunction* startF;
	if (!GetSelectedFunction(&startF))
		{
		startF = itsActiveUIF;
		}

	JNaryFunction* targetF;
	JIndex newArgIndex;
	return JConvertToBoolean(
			startF != NULL && GetCommaTarget(startF, &targetF, &newArgIndex));
}

/******************************************************************************
 GetCommaTarget (private)

	Search up the tree for a function to apply the comma operator to.
	We only apply commas to JNaryFunctions, not JNaryOperators.

 ******************************************************************************/

JBoolean
JExprEditor::GetCommaTarget
	(
	JFunction*		startF,
	JNaryFunction**	targetF,
	JIndex*			newArgIndex
	)
{
	if (startF == itsFunction)
		{
		return kJFalse;
		}

	JExprNodeList nodeList(itsFunction);

	JFunction* currF = startF;
	JFunction* parentF = NULL;
	while (currF != itsFunction)
		{
		parentF = GetParentFunction(currF, nodeList);
		JNaryFunction* naryParentF = parentF->CastToJNaryFunction();
		if (naryParentF != NULL && parentF->CastToJNaryOperator() == NULL)
			{
			*targetF = naryParentF;
			const JBoolean found = naryParentF->FindArg(currF, newArgIndex);
			assert( found );
			(*newArgIndex)++;	// insert after current arg
			return kJTrue;
			}
		currF = parentF;
		}

	// falling through means that we can't find an nary function

	return kJFalse;
}

/******************************************************************************
 SendKeyToActiveUIF (private)

 ******************************************************************************/

void
JExprEditor::SendKeyToActiveUIF
	(
	const JCharacter key
	)
{
	if (itsUndoFunction == NULL)
		{
		SaveStateForUndo();
		}

	JBoolean needParse, needRender;
	JBoolean changed = itsActiveUIF->HandleKeyPress(key, &needParse, &needRender);

	JFunction* newF = NULL;
	JUserInputFunction* newUIF = NULL;
	if (needParse && itsActiveUIF->Parse(&newF, &newUIF, &needRender))
		{
		ReplaceFunction(itsActiveUIF, newF);
		itsActiveUIF = NULL;
		Render();
		ActivateUIF(newUIF);
		}
	else if (needRender)
		{
		JUserInputFunction* savedUIF = itsActiveUIF;
		itsActiveUIF = NULL;
		Render();
		itsActiveUIF = savedUIF;
		}
	else if (changed)
		{
		EIPRefresh();
		}
}

/******************************************************************************
 ActivateUIF (private)

 ******************************************************************************/

void
JExprEditor::ActivateUIF
	(
	JUserInputFunction* uif
	)
{
	if (itsActiveFlag && uif != NULL && uif != itsActiveUIF && EndEditing())
		{
		itsActiveUIF = uif;
		uif->Activate();
		itsSelection = 0;

		JIndex uifIndex;
		const JBoolean found = itsRectList->FindFunction(uif, &uifIndex);
		assert( found );
		if (!EIPScrollToRect(itsRectList->GetRect(uifIndex)))
			{
			EIPRefresh();
			}
		}
}

/******************************************************************************
 ActivateNextUIF

 ******************************************************************************/

void
JExprEditor::ActivateNextUIF()
{
	JUserInputFunction* nextUIF = FindNextUIF(itsActiveUIF);
	if (nextUIF != itsActiveUIF)
		{
		ActivateUIF(nextUIF);
		}
	else if (itsActiveUIF != NULL && !itsActiveUIF->IsEmpty())
		{
		EndEditing();
		}
	// if itsActiveUIF is empty, leave it active
}

/******************************************************************************
 DeleteFunction (private)

	Returns the sensible selection after deleting the target function.

 ******************************************************************************/

JFunction*
JExprEditor::DeleteFunction
	(
	JFunction* targetF
	)
{
	if (targetF == itsFunction)
		{
		PrivateClearFunction();
		return itsFunction;
		}

	JExprNodeList nodeList(itsFunction);

	JFunction* parentF = GetParentFunction(targetF, nodeList);
	JFunctionWithArgs* parentfwa = parentF->CastToJFunctionWithArgs();
	if (parentfwa == NULL || parentfwa->GetArgCount() == 1)
		{
		JFunction* currentF = parentF;
		while (1)
			{
			if (currentF == itsFunction)
				{
				PrivateClearFunction();
				return itsFunction;
				}
			JFunction* ancestorF = GetParentFunction(currentF, nodeList);
			parentfwa = ancestorF->CastToJFunctionWithArgs();
			if (parentfwa != NULL && parentfwa->GetArgCount() > 1)
				{
				targetF = currentF;
				parentF = ancestorF;
				break;
				}
			else
				{
				currentF = ancestorF;
				}
			}
		}

	JFunction* fToSelect = NULL;

	const JSize parentArgCount = parentfwa->GetArgCount();
	if (parentArgCount == 2)
		{
		JFunction* arg1 = parentfwa->GetArg(1);
		JFunction* arg2 = parentfwa->GetArg(2);
		JFunction* savedF = NULL;
		if (arg1 == targetF)
			{
			savedF = arg2->Copy();
			}
		else
			{
			assert( arg2 == targetF );
			savedF = arg1->Copy();
			}
		ReplaceFunction(parentF, savedF);
		fToSelect = savedF;
		}
	else if (parentArgCount > 2)
		{
		JNaryFunction* naryF = parentF->CastToJNaryFunction();
		assert( naryF != NULL );
		const JBoolean ok = naryF->DeleteArg(targetF);
		assert( ok );
		fToSelect = naryF;
		}

	return fToSelect;
}

/******************************************************************************
 ReplaceFunction (private)

	This is not appropriate for JExprNodeList because we can assume that
	the parent is always a JFunction and never a JDecision.

 ******************************************************************************/

void
JExprEditor::ReplaceFunction
	(
	JFunction* origF,
	JFunction* newF
	)
{
	if (origF == itsFunction)
		{
		jdelete itsFunction;
		itsFunction = newF;
		}
	else
		{
		JFunction* parentF     = GetParentFunction(origF);
		JFunctionWithArgs* fwa = parentF->CastToJFunctionWithArgs();
		JFunctionWithVar* fwv  = parentF->CastToJFunctionWithVar();
		if (fwa != NULL)
			{
			const JBoolean ok = fwa->ReplaceArg(origF, newF);
			assert( ok );
			}
		else
			{
			assert( fwv != NULL );
			assert( fwv->GetArrayIndex() == origF );
			fwv->SetArrayIndex(newF);
			}
		}
}

/******************************************************************************
 GetParentFunction (private)

	This is not appropriate for JExprNodeList because we can assume that
	the parent is always a JFunction and never a JDecision.

	The caller -can- assume that the result is either a JFunctionWithArgs
	or a JFunctionWithVar.

	*** Do not call this on the root node (itsFunction)

 ******************************************************************************/

JFunction*
JExprEditor::GetParentFunction
	(
	JFunction* f
	)
	const
{
	JExprNodeList nodeList(itsFunction);
	return GetParentFunction(f, nodeList);
}

JFunction*
JExprEditor::GetParentFunction
	(
	JFunction*				f,
	const JExprNodeList&	nodeList
	)
	const
{
	JIndex fIndex;
	const JBoolean found = nodeList.Find(f, &fIndex);
	assert( found );

	const JIndex parentIndex = nodeList.GetParentIndex(fIndex);
	assert( parentIndex != kJExprRootNode );

	JFunction* parentF = nodeList.GetFunction(parentIndex);
	assert( parentF->CastToJFunctionWithArgs() != NULL ||
			parentF->CastToJFunctionWithVar()  != NULL );

	return parentF;
}

/******************************************************************************
 FindNextUIF (private)

 ******************************************************************************/

JUserInputFunction*
JExprEditor::FindNextUIF
	(
	JUserInputFunction* currUIF
	)
	const
{
	JIndex currentIndex = 0;
	if (currUIF != NULL)
		{
		const JBoolean found = itsRectList->FindFunction(currUIF, &currentIndex);
		assert( found );
		}

	const JSize rectCount = itsRectList->GetElementCount();
	{
	for (JIndex i=currentIndex+1; i<=rectCount; i++)
		{
		const JFunction* f = itsRectList->GetFunction(i);
		if (f->GetType() == kJUserInputType)
			{
			const JUserInputFunction* uif = dynamic_cast<const JUserInputFunction*>(f);
			assert( uif != NULL );
			return const_cast<JUserInputFunction*>(uif);
			}
		}
	}
	{
	for (JIndex i=1; i<=currentIndex; i++)
		{
		const JFunction* f = itsRectList->GetFunction(i);
		if (f->GetType() == kJUserInputType)
			{
			const JUserInputFunction* uif = dynamic_cast<const JUserInputFunction*>(f);
			assert( uif != NULL );
			return const_cast<JUserInputFunction*>(uif);
			}
		}
	}

	return NULL;
}

/******************************************************************************
 Render (private)

 ******************************************************************************/

void
JExprEditor::Render()
{
	assert( itsFunction != NULL );
	assert( itsActiveUIF == NULL );		// forces caller to think about this issue

	itsSelection = 0;

	jdelete itsRectList;
	itsRectList = jnew JExprRectList;
	assert( itsRectList != NULL );

	// calculate the locations of everything

	JPoint upperLeft(0,0);
	itsFunction->PrepareToRender(*this, upperLeft, GetInitialFontSize(), itsRectList);

	// tell whether or not we now need the tab key

	EIPAdjustNeedTab(ContainsUIF());

	// adjust bounds

	EIPBoundsChanged();

	// redraw the expression

	EIPRefresh();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JExprEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == const_cast<JVariableList*>(itsVarList) &&
		message.Is(JVariableList::kVarNameChanged))
		{
		JIndex savedSel = itsSelection;
		JUserInputFunction* savedUIF = itsActiveUIF;
		itsActiveUIF = NULL;
		Render();
		itsActiveUIF = savedUIF;
		SetSelection(savedSel);
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ExprRenderer routines

	Not inline because they are virtual

	We don't check for itsPainter!=NULL because it should never happen and
	would waste a lot of time.

 ******************************************************************************/

/******************************************************************************
 Font size

 ******************************************************************************/

JSize
JExprEditor::GetInitialFontSize()
	const
{
	return JGetDefaultFontSize();
}

JSize
JExprEditor::GetSuperSubFontSize
	(
	const JSize baseFontSize
	)
	const
{
	const JSize size = JGetDefaultFontSize();
	if (baseFontSize == size)
		{
		return size-2;
		}
	else
		{
		return size-4;
		}
}

/******************************************************************************
 GetLineHeight

 ******************************************************************************/

JSize
JExprEditor::GetLineHeight
	(
	const JSize fontSize
	)
	const
{
	const JFont font = itsFontManager->GetFont(JGetDefaultFontName(), fontSize, itsDefaultStyle);
	return font.GetLineHeight();
}

/******************************************************************************
 Strings

 ******************************************************************************/

JSize
JExprEditor::GetStringWidth
	(
	const JSize			fontSize,
	const JCharacter*	str
	)
	const
{
	const JFont font = itsFontManager->GetFont(JGetDefaultFontName(), fontSize, itsDefaultStyle);
	return font.GetStringWidth(str);
}

void
JExprEditor::DrawString
	(
	const JCoordinate	left,
	const JCoordinate	midline,
	const JSize			fontSize,
	const JCharacter*	str
	)
	const
{
	const JCoordinate h = GetLineHeight(fontSize);
	JCoordinate y = midline - h/2;
	itsPainter->SetFont(itsFontManager->GetFont(JGetDefaultFontName(), fontSize, itsDefaultStyle));
	itsPainter->String(left,y, str);
}

/******************************************************************************
 Greek characters

 ******************************************************************************/

JSize
JExprEditor::GetGreekCharWidth
	(
	const JSize			fontSize,
	const JCharacter	c
	)
	const
{
	const JCharacter str[2] = {c, 0};

	const JFont font = itsFontManager->GetFont(JGetGreekFontName(), fontSize, itsDefaultStyle);
	return font.GetStringWidth(str);
}

void
JExprEditor::DrawGreekCharacter
	(
	const JCoordinate	left,
	const JCoordinate	midline,
	const JSize			fontSize,
	const JCharacter	c
	)
	const
{
	const JCoordinate h = GetLineHeight(fontSize);
	JCoordinate y = midline - h/2;
	itsPainter->SetFont(itsFontManager->GetFont(JGetGreekFontName(), fontSize, itsDefaultStyle));

	const JCharacter str[2] = {c, 0};
	itsPainter->String(left,y, str);
}

/******************************************************************************
 Horizontal bar

 ******************************************************************************/

JSize
JExprEditor::GetHorizBarHeight()
	const
{
	return 3;
}

void
JExprEditor::DrawHorizBar
	(
	const JCoordinate	left,
	const JCoordinate	v,
	const JSize			length
	)
	const
{
	const JCoordinate y = v+1;
	itsPainter->Line(left,y, left+length-1,y);
}

/******************************************************************************
 Vertical bar

 ******************************************************************************/

JSize
JExprEditor::GetVertBarWidth()
	const
{
	return 3;
}

void
JExprEditor::DrawVertBar
	(
	const JCoordinate	h,
	const JCoordinate	top,
	const JSize			length
	)
	const
{
	const JCoordinate x = h+1;
	itsPainter->Line(x,top, x,top+length-1);
}

/******************************************************************************
 GetSuperscriptHeight

 ******************************************************************************/

JSize
JExprEditor::GetSuperscriptHeight
	(
	const JCoordinate baseHeight
	)
	const
{
	return JRound(baseHeight/2.0);
}

/******************************************************************************
 GetSubscriptDepth

 ******************************************************************************/

JSize
JExprEditor::GetSubscriptDepth
	(
	const JCoordinate baseHeight
	)
	const
{
	return JRound(baseHeight/2.0);
}

/******************************************************************************
 Parentheses

 ******************************************************************************/

const long kDegParenAngle = 30;
const JFloat kParenAngle  = kDegParenAngle * kJDegToRad;

JSize
JExprEditor::GetParenthesisWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 2+JRound(0.5 + (argHeight * (1.0 - cos(kParenAngle)))/(2.0 * sin(kParenAngle)));
}

void
JExprEditor::DrawParentheses
	(
	const JRect& argRect
	)
	const
{
	const JFloat r = argRect.height()/(2.0*sin(kParenAngle));
	const JCoordinate r1  = JRound(r);
	const JCoordinate dxc = JRound(r*cos(kParenAngle));
	const JCoordinate xcl = argRect.left  + dxc - 2;
	const JCoordinate xcr = argRect.right - dxc + 1;
	const JCoordinate yt  = argRect.ycenter() - r1;

	itsPainter->Arc(xcl-r1, yt, 2*r1, 2*r1, 180-kDegParenAngle,  2*kDegParenAngle);
	itsPainter->Arc(xcr-r1, yt, 2*r1, 2*r1,     kDegParenAngle, -2*kDegParenAngle);
}

/******************************************************************************
 Square brackets

 ******************************************************************************/

JSize
JExprEditor::GetSquareBracketWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 3+((argHeight-1)/10);
}

void
JExprEditor::DrawSquareBrackets
	(
	const JRect& argRect
	)
	const
{
	const JCoordinate h = argRect.height();
	const JCoordinate w = GetSquareBracketWidth(h)-2;

	itsPainter->SetPenLocation(argRect.left-2, argRect.top);
	itsPainter->DeltaLine(-w,0);
	itsPainter->DeltaLine( 0,h-1);
	itsPainter->DeltaLine( w,0);

	itsPainter->SetPenLocation(argRect.right+1, argRect.top);
	itsPainter->DeltaLine( w,0);
	itsPainter->DeltaLine( 0,h-1);
	itsPainter->DeltaLine(-w,0);
}

/******************************************************************************
 Square root

	tan(60) = sqrt(3)

 ******************************************************************************/

JSize
JExprEditor::GetSquareRootLeadingWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 1+JRound(2.0*argHeight/(4.0*sqrt(3.0)));
}

JSize
JExprEditor::GetSquareRootTrailingWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 3;
}

JSize
JExprEditor::GetSquareRootExtraHeight()
	const
{
	return 4;
}

void
JExprEditor::DrawSquareRoot
	(
	const JRect& enclosure
	)
	const
{
	const JCoordinate x = enclosure.left;
	const JCoordinate y = enclosure.top + JRound(3.0*enclosure.height()/4.0);
	const JCoordinate w = JRound((enclosure.height()-3)/(4.0*sqrt(3.0)));

	itsPainter->SetPenLocation(x,y);
	itsPainter->DeltaLine(w,enclosure.bottom-1-y);
	itsPainter->DeltaLine(w,-enclosure.height()+3);
	itsPainter->DeltaLine(enclosure.width()-2*w-1,0);
	itsPainter->DeltaLine(0,JRound(enclosure.height()/8.0));
}
