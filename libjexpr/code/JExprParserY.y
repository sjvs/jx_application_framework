%{
/*
Copyright (C) 2018 by John Lindal.
*/

#include "JAbsValue.h"
#include "JAlgSign.h"
#include "JArcCosine.h"
#include "JArcHypCosine.h"
#include "JArcHypSine.h"
#include "JArcHypTangent.h"
#include "JArcSine.h"
#include "JArcTangent.h"
#include "JArcTangent2.h"
#include "JConjugate.h"
#include "JConstantValue.h"
#include "JCosine.h"
#include "JDivision.h"
#include "JExponent.h"
#include "JHypCosine.h"
#include "JHypSine.h"
#include "JHypTangent.h"
#include "JImagPart.h"
#include "JLogB.h"
#include "JLogE.h"
#include "JMaxFunc.h"
#include "JMinFunc.h"
#include "JNamedConstant.h"
#include "JNegation.h"
#include "JParallel.h"
#include "JPhaseAngle.h"
#include "JProduct.h"
#include "JRealPart.h"
#include "JRotateComplex.h"
#include "JRoundToInt.h"
#include "JSine.h"
#include "JSquareRoot.h"
#include "JSummation.h"
#include "JTangent.h"
#include "JTruncateToInt.h"
#include "JUserInputFunction.h"
#include "JVariableValue.h"
#include <JPtrArray-JString.h>

// also uncomment yydebug=1; below
//#define YYERROR_VERBOSE
//#define YYDEBUG 1
%}

%union {
	JString*				pString;
	JFunction*				pFunction;
	JPtrArray<JFunction>*	pList;
}

%{
#define NOTAB
#include "JExprParser.h"

#define yyparse JExprParser::yyparse
%}

%pure-parser

%token<pString>	P_NUMBER P_VARIABLE
%token			P_PI P_E P_I
%token			P_INPUT
%token			P_FN_ABS P_FN_ARCCOS P_FN_ARCCOSH P_FN_ARCSIN P_FN_ARCSINH
%token			P_FN_ARCTAN P_FN_ARCTAN2 P_FN_ARCTANH P_FN_CONJUGATE
%token			P_FN_COS P_FN_COSH P_FN_IM P_FN_LN P_FN_LOG P_FN_LOG2
%token			P_FN_LOG10 P_FN_MAX P_FN_MIN P_FN_PHASE P_FN_RE
%token			P_FN_ROTATE P_FN_SIN P_FN_SINH P_FN_SQRT P_FN_TAN P_FN_TANH
%token			P_FN_SIGN P_FN_ROUND P_FN_TRUNCATE P_FN_PARALLEL
%token			P_EOF

%type<pFunction>	expression e
%type<pList>		arglist

%{

// debugging output

#define YYPRINT(file, type, value)	yyprint(file, type, value)
#include <stdio.h>

inline void
yyprint
	(
	FILE*	file,
	int		type,
	YYSTYPE	value
	)
{
	if (value.pString != NULL)
		{
		fprintf(file, "string:  %s", (value.pString)->GetBytes());
		}
}

%}

%{
#include <jAssert.h>	// must be last
%}

%initial-action
{
//	yydebug = 1;
}

/* operator associations and precedence */

%left '+' '-'
%left '*' '/'
%right '^'
%left UMINUS

%%

expression
	: e P_EOF
		{itsCurrentNode = $1; YYACCEPT;}
	;

e
	: P_NUMBER
		{$$ = jnew JConstantValue(*$1);}
	| P_E
		{$$ = jnew JNamedConstant(kEJNamedConstIndex);}
	| P_PI
		{$$ = jnew JNamedConstant(kPiJNamedConstIndex);}
	| P_I
		{$$ = jnew JNamedConstant(kIJNamedConstIndex);}
	| P_VARIABLE
		{$$ = jnew JVariableValue(*$1);}
	| P_INPUT
		{$$ = jnew JUserInputFunction();}

	| '(' e ')'
		{$$ = $2;}

	| e '+' e
		{$$ = yy.MathFunction.updateSum($1, $3);}
	| e '-' e
		{$$ = yy.MathFunction.updateSum($1, jnew yy.MathFunction.Negate($3));}
	| e '*' e
		{$$ = yy.MathFunction.updateProduct($1, $3);}
	| e '/' e
		{$$ = jnew JDivision($1, $3);}
	| e '^' e
		{$$ = jnew JExponent($1, $3);}
	| '-' e %prec UMINUS
		{$$ = JNegation($2);}

	| P_FN_ABS '(' e ')'
		{$$ = jnew JAbsValue($3);}
	| P_FN_PHASE '(' e ')'
		{$$ = jnew JPhaseAngle($3);}
	| P_FN_CONJUGATE '(' e ')'
		{$$ = jnew JConjugate($3);}
	| P_FN_ROTATE '(' e ',' e ')'
		{$$ = jnew JRotateComplex($3, $5);}
	| P_FN_RE '(' e ')'
		{$$ = jnew JRealPart($3);}
	| P_FN_IM '(' e ')'
		{$$ = jnew JImagPart($3);}

	| P_FN_MIN '(' arglist ')'
		{$$ = jnew JMinFunc($3);}
	| P_FN_MAX '(' arglist ')'
		{$$ = jnew JMaxFunc($3);}

	| P_FN_SQRT '(' e ')'
		{$$ = jnew JSquareRoot($3);}
	| P_FN_PARALLEL '(' arglist ')'
		{$$ = jnew JParallel($3);}

	| P_FN_SIGN '(' e ')'
		{$$ = jnew JAlgSign($3);}
	| P_FN_ROUND '(' e ')'
		{$$ = jnew JRoundToInt($3);}
	| P_FN_TRUNCATE '(' e ')'
		{$$ = jnew JTruncateToInt($3);}

	| P_FN_LOG '(' e ',' e ')'
		{$$ = jnew JLogB($3, $5);}
	| P_FN_LOG2 '(' e ')'
		{$$ = jnew JLogB(jnew JConstantValue(2), $3);}
	| P_FN_LOG10 '(' e ')'
		{$$ = jnew JLogB(jnew JConstantValue(10), $3);}
	| P_FN_LN '(' e ')'
		{$$ = jnew JLogE($3);}

	| P_FN_ARCSIN '(' e ')'
		{$$ = jnew JArcSine($3);}
	| P_FN_ARCCOS '(' e ')'
		{$$ = jnew JArcCosine($3);}
	| P_FN_ARCTAN '(' e ')'
		{$$ = jnew JArcTangent($3);}
	| P_FN_ARCTAN2 '(' e ',' e ')'
		{$$ = jnew JArcTangent2($3, $5);}

	| P_FN_SIN '(' e ')'
		{$$ = jnew JSine($3);}
	| P_FN_COS '(' e ')'
		{$$ = jnew JCosine($3);}
	| P_FN_TAN '(' e ')'
		{$$ = jnew JTangent($3);}

	| P_FN_SINH '(' e ')'
		{$$ = jnew JHypSine($3);}
	| P_FN_COSH '(' e ')'
		{$$ = jnew JHypCosine($3);}
	| P_FN_TANH '(' e ')'
		{$$ = jnew JHypTangent($3);}

	| P_FN_ARCSINH '(' e ')'
		{$$ = jnew JArcHypSine($3);}
	| P_FN_ARCCOSH '(' e ')'
		{$$ = jnew JArcHypCosine($3);}
	| P_FN_ARCTANH '(' e ')'
		{$$ = jnew JArcHypTangent($3);}
	;

arglist
	: e
		{$$ = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll); $$->AppendElement($1);}
	| arglist ',' e
		{$$ = $1; $$->AppendElement($3);}
	;
