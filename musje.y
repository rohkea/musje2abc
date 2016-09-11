%{
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "musje2abc.h"
#include "read123.h"
#include "writeabc.h"

int yydebug=1;

void yyerror(const char *str) {
	fprintf(stderr, "error: %s\n", str);
}

int yywrap() {
	return 1;
}

int main(int argc, const char **argv) {
	init_convertor();
	yyparse();
	display_result();
}
%}

%token NOTEVALUE
%token TOKACCIDENTAL_SHARP TOKACCIDENTAL_FLAT TOKACCIDENTAL_NATURAL
%token TOKQUAVER TOKSEMIQUAVER TOKLENGTHENER
%token TOKOCTAVE_UP TOKOCTAVE_DOWN
%token TOKDOT TOKTIE TOKOPENSLUR TOKCLOSESLUR
%token TOKBAR_SINGLE TOKBAR_DOUBLE TOKBAR_END
%token TOKBAR_REPEAT_BEGIN TOKBAR_REPEAT_END TOKBAR_REPEAT_BOTH
%token BARLENGTH
%token TOKEOL TOKWHITESPACE
%token HEADER
%token TRANSPOSE_MAJOR_NATURAL TRANSPOSE_MAJOR_FLAT TRANSPOSE_MAJOR_SHARP
%token COMMENT_MULTILINE COMMENT_SINGLELINE

%%

tune:
    	header
    	lines
	;

header:
         /* empty */
	| HEADER
	{
		read_header(currentHeader);
	}
	;

lines:
     	  line
	| lines TOKEOL
	  {
		read_newline();
	  }
	  line
	;

line:     /* empty */
	| line note
	| line bar
	| line bar_length
	| line open_slur
	| line close_slur
	| line transposition
	| line comment
	;

note:
	accidental NOTEVALUE octave note_length note_dots tied el_separator
	{
		read_note($1, $2, $3, $4, $5, $6, $7);
	}
	;

open_slur:
	TOKOPENSLUR el_separator
	{
		read_slur(0);
	};

close_slur:
	TOKCLOSESLUR el_separator
	{
		read_slur(1);
	};

bar:
   	  single_bar el_separator
	| double_bar el_separator
	| repeat_begin_bar el_separator
	| repeat_end_bar el_separator
	| repeat_both_bar el_separator
	| end_bar el_separator
	;

single_bar:
	TOKBAR_SINGLE
	{
		read_bar(BT_SINGLE);
	};

double_bar:
	TOKBAR_DOUBLE
	{
		read_bar(BT_DOUBLE);
	};

repeat_begin_bar:
	TOKBAR_REPEAT_BEGIN
	{
		read_bar(BT_REPEAT_BEGIN);
	};

repeat_end_bar:
	TOKBAR_REPEAT_END
	{
		read_bar(BT_REPEAT_END);
	};

repeat_both_bar:
	TOKBAR_REPEAT_BOTH
	{
		read_bar(BT_REPEAT_BOTH);
	};

end_bar:
	TOKBAR_END
	{
		read_bar(BT_END);
	};

bar_length:
	BARLENGTH el_separator
	{
		read_bar_length(barLengthNumerator, barLengthDenominator);	
	}

transposition:
	  transposition_major_natural el_separator 
	| transposition_major_flat el_separator
	| transposition_major_sharp el_separator
	;

transposition_major_natural:
	TRANSPOSE_MAJOR_NATURAL	
	{
		read_major_scale_transposition($1, 0);
	};

transposition_major_sharp:
	TRANSPOSE_MAJOR_SHARP
	{
		read_major_scale_transposition($1, 1);
	};

transposition_major_flat:
	TRANSPOSE_MAJOR_FLAT
	{
		read_major_scale_transposition($1, -1);
	};

comment:
	  COMMENT_SINGLELINE el_separator
	| COMMENT_MULTILINE el_separator
	;

octave:
     	{
		$$ = 0;
	}
	  /* empty */
	| octave_lowerers
	| octave_raisers;

octave_lowerers:
	{
		$$ = -1;
	}
	TOKOCTAVE_DOWN
	| octave_lowerers TOKOCTAVE_DOWN
	{
		$$--;
	}
	;

octave_raisers:
	{
		$$ = 1;
	}
	TOKOCTAVE_UP
	| octave_raisers TOKOCTAVE_UP
	{
		$$++;
	}
	;

tied:

	  /* empty */
	{
		$$ = 0;
	}
	| TOKTIE
	{
		$$ = 1;
	}
	;

el_separator:
	{
		$$ = 0;
	}
	  /* empty */
	| TOKWHITESPACE
	{
		$$ = 1;
	}
	;

accidental:
	{
		$$ = DEFAULT_ACCIDENTAL; /* use previously defined accidental */
	}
	  /* empty */
	| accidental_sharp
        | accidental_flat
	| accidental_natural
	;

accidental_sharp:
	TOKACCIDENTAL_SHARP
	{
		$$ = 1;
	}
	;

accidental_flat:
	TOKACCIDENTAL_FLAT
	{
		$$ = -1;
	}
	;

accidental_natural:
	TOKACCIDENTAL_NATURAL
	{
		$$ = 0;
	}
	;

note_length:
	  note_length_quaver
	| note_length_semiquaver
	| note_length_crotchets
	;

note_length_quaver:
	TOKQUAVER
	{
		$$ = LENGTH_QUAVER;
	}
	;

note_length_semiquaver:
	TOKSEMIQUAVER
	{
		$$ = LENGTH_SEMIQUAVER;
	}
	;

note_length_crotchets:
	/* empty */
	{
		$$ = LENGTH_CROTCHET;
	}
	| note_length_crotchets TOKLENGTHENER
	{
		$$ += LENGTH_CROTCHET;
	}
	;

note_dots:
	{
		$$ = 0;
	}
	  /* empty */
	|
	note_dots '.'
	{
		$$ += 1;
	}
	;
