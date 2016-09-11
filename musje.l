%{
#include <stdio.h>
#include "y.tab.h"
#include "common.h"
#include "musje2abc.h"
%}

%%
\/\*.*\*\/	return COMMENT_MULTILINE;
\/\/.*\n	return COMMENT_SINGLELINE;
[ \t　]*<<.*>>.*\n	if (NULL != currentHeader) { free(currentHeader); } currentHeader = strdup(yytext); return HEADER;
\/1=b[A-G]		yylval = yytext[4] >= 'C' ? yytext[4] - 'C' : yytext[4] - 'A' + NOTE_A; return TRANSPOSE_MAJOR_FLAT;
\/1=#[A-G]		yylval = yytext[4] >= 'C' ? yytext[4] - 'C' : yytext[4] - 'A' + NOTE_A; return TRANSPOSE_MAJOR_SHARP;
\/1=[A-G]		yylval = yytext[3] >= 'C' ? yytext[3] - 'C' : yytext[3] - 'A' + NOTE_A; return TRANSPOSE_MAJOR_NATURAL;
[0-9][0-9]*\/[0-9][0-9]*	sscanf(yytext, "%d/%d", &barLengthNumerator, &barLengthDenominator); return BARLENGTH;
[01-7]		yylval = yytext[0] - '0'; return NOTEVALUE;
♭|b		return TOKACCIDENTAL_FLAT;
♮|n		return TOKACCIDENTAL_NATURAL;
#|♯		return TOKACCIDENTAL_SHARP;
_		return TOKQUAVER;
=		return TOKSEMIQUAVER;
[ \t　]*-	return TOKLENGTHENER;
'		return TOKOCTAVE_UP;
,		return TOKOCTAVE_DOWN;
~		return TOKTIE;
\(		return TOKOPENSLUR;
\)		return TOKCLOSESLUR;
:\|:		return TOKBAR_REPEAT_BOTH;
\|:		return TOKBAR_REPEAT_BEGIN;
:\|		return TOKBAR_REPEAT_END;
\|\]		return TOKBAR_END;
\|\|		return TOKBAR_DOUBLE;
\|		return TOKBAR_SINGLE;
\n		return TOKEOL;
[ \t　]+	return TOKWHITESPACE;
.		return (int)yytext[0];
%%
