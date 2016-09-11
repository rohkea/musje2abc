#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "writeabc.h"

int currentKey = NOTE_C;
int currentNoteLength = LENGTH_QUAVER;
int currentTuneNumber = 1;
int keyPrinted = 0;
static signed char majorScaleIntervals[7] = {2, 2, 1, 2, 2, 2, 1};
static signed char currentKeySignature[7] = {0, 0, 0, 0, 0, 0, 0};
static signed char currentAccidentals[8][7] = {
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0}
};
int startedNoteOutput = 0;

/**
 * @param note_key absolute note value (0 is C, 6 is A)
 * @return 0 for natural, -1 for flat, 1 for sharp
 */
int get_key_singature_semitone_shift(int note_key) {
	assert(note_key >= 0 && note_key < 7);
	return currentKeySignature[note_key];
}

int set_current_key(int key, int semitone_shift) {
	int i, note, prevNote, neededDistance, defaultDistance,
		shiftDirection, dir;
	
	assert(key >= NOTE_C && key <= NOTE_B);
	assert(semitone_shift >= -1 && semitone_shift <= 1);
	currentKey = key;
	shiftDirection = semitone_shift;
	
	/* Build key signature; keys should have 2-2-1-2-2-2-1 distances */
	currentKeySignature[key] = semitone_shift;
	for (i = 1; i < 7; i++) {
		note = (i + key) % 7;
		prevNote = (i + key + 6) % 7;
		neededDistance = majorScaleIntervals[i - 1];
		defaultDistance = majorScaleIntervals[prevNote]
					- currentKeySignature[prevNote];
		dir = neededDistance - defaultDistance;
		if (dir != 0 && shiftDirection == 0) {
			shiftDirection = dir;
		}
		else if (shiftDirection != 0 && dir != 0
					&& dir != shiftDirection) {
			fprintf(stderr, "Impossible key signature: %c%s\n",
				key < NOTE_A ? 'C' + key : 'A' + key - NOTE_A,
				semitone_shift == -1 ? "b"
				: semitone_shift == 1 ? "#" : "");
			/* exit(-1); */
		}
		
		currentKeySignature[note] = dir;
	}
}

void reset_current_accidentals() {
	int octave, note;

	for (octave = 0; octave < 8; octave++) {
		for (note = NOTE_C; note <= NOTE_A; note++) {
			currentAccidentals[octave][note] =
				currentKeySignature[note];
		}
	}
}



int lookahead_has_notes(SheetElement *sheet, int from, int len) {
	int j;

	j = from;
	while (j < len && SET_NOTE != sheet[j].type) {
		j++;
	}
	
	return j < len && SET_NOTE == sheet[j].type;
}

static char *abcAccidentals[] = {"__", "_", "=", "^", "^^"};

void print_key_signature(SheetElement *sheet, int *i, int len) {
	const char *sign;
	
	if (startedNoteOutput) {
		if (lookahead_has_notes(sheet, *i, len)) {
			printf(" \\");
		}
		printf("\n");
		startedNoteOutput = 0;
	}
	
	sign = "";
	if (currentKeySignature[currentKey] == -1) {
		sign = "b";
	}
	if (currentKeySignature[currentKey]== 1) {
		sign = "#";
	}
	printf("K:%c%s\n",
		currentKey < 5 ? 'C' + currentKey
			: 'A' + currentKey - 5,
		sign);

	reset_current_accidentals();

	keyPrinted = 1;
}

void print_note(Note *n) {
	int octave, letter, shift, accidental,
	    expectedShift, needAccidental, i;
	char letterC;

	if (n->number + currentKey <= 7) {
		octave = n->octave;
		letter = n->number + currentKey - 1;
	}
	else {
		octave = n->octave + 1;
		letter = n->number + currentKey - 8;
	}
	
	shift = get_key_singature_semitone_shift(letter) +
		+ n->semitone_shift;
	/* TODO: do something if |shift| = 3
	   (this can happen if the key signature has b/# and
	    the note has a double bb/##) */
	assert(shift >= -2 && shift <= 2);

	expectedShift = currentAccidentals[octave][letter];
	if (expectedShift != shift) {
		/* We can't just rely on DEFAULT_ACCIDENTAL for determining
		 * whether accidental is needed, because Musje123 accidentals
		 * span all octaves, while ABC accidentals span just one
		 * octave. So it's easily possible to have an accidental that
		 * is needed in ABC but not in Musje, or vice versa.
		 */
		currentAccidentals[octave][letter] = shift;
		needAccidental = 1;
	}
	else {
		needAccidental = DEFAULT_ACCIDENTAL != n->accidental;
	}
	
	if (octave <= ABC_MIDDLE_OCTAVE) {
		letterC = letter < NOTE_A ? letter + 'C'
			: letter + 'A' - NOTE_A;
	}
	else {
		letterC = letter < NOTE_A ? letter + 'c'
			: letter + 'a' - NOTE_A;
	}
	printf("%s%c",
		needAccidental ? abcAccidentals[shift + 2] : "",
		letterC);
	if (octave < ABC_MIDDLE_OCTAVE) {
		for (i = octave; i < ABC_MIDDLE_OCTAVE; i++) {
			printf(",");
		}
	}
	if (octave > ABC_MIDDLE_OCTAVE + 1) {
		for (i = octave; i > ABC_MIDDLE_OCTAVE + 1; i--) {
			printf("'");
		}
	}
}

/** Debugging function. Likely to be removed. */
void print_current_key(void) {
	int i;
	const char *sign;
	
	sign = "";
	if (currentKeySignature[currentKey] == -1) {
		sign = "b";
	}
	if (currentKeySignature[currentKey]== 1) {
		sign = "#";
	}
	printf("%% Key: %c%s\n%% ",
		currentKey < 5 ? 'C' + currentKey
			: 'A' + currentKey - 5,
		sign);
	
	for (i = 0; i < 7; i++) {
		sign = "";
		if (currentKeySignature[i] == -1) {
			sign = "b";
		}
		if (currentKeySignature[i]== 1) {
			sign = "#";
		}
		printf("%c%s ", i < 5 ? 'C' + i : 'A' + i - 5, sign);
	}
	printf("\n");
}

void print_note_length(Note *note) {
	int numerator, denominator;
	int i;

	if (note->length > currentNoteLength) {
		numerator = note->length / currentNoteLength;
		denominator = 1;
	}
	else {
		numerator = 1;
		denominator = currentNoteLength / note->length;
	}
	
	/* TODO: check if dots are correct */
	for (i = 0; i < note->dots; i++) {
		numerator = numerator + 1;
		denominator = 2*denominator;
	}
	
	if (1 == numerator && 1 == denominator) {
		/* do nothing */
	}
	if (1 == denominator) {
		printf("%d", numerator);
	}
	else if (1 == numerator) {
		printf("/%d", denominator);
	}
	else {
		printf("%d/%d", numerator, denominator);
	}
}

/**
 * If sheet[*i] contains a /1=N transposition, then displays it and changes
 * note values accordingly, adds 1 to *i, and returns a non-zero value.
 * Otherwise, returns 0.
 *
 * @param sheet List of SheetElements.
 * @param i Pointer to the current index of SheetElements. Will be modified if return value is non-zero.
 * @param len Number of SheetElements.
 *
 * @return A non-zero value if sheet[*i] contains a transposition and that transposition was displayed. Otherwise, zero.
 */
int do_transposition_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;

	e = &sheet[*i];
	if (SET_TRANSPOSITION == e->type) {
		set_current_key(e->transposition.note_letter,
				e->transposition.semitone_shift);
		print_key_signature(sheet, i, len);

		startedNoteOutput = 0;
		
		*i += 1;
		return 1;
	}

	return 0;
}

/**
 * If sheet[*i] contains a time signature (bar length), then displays it,
 * adds 1 to *i, and returns a non-zero value. Otherwise, returns 0.
 *
 * @param sheet List of SheetElements.
 * @param i Pointer to the current index of SheetElements. Will be modified if return value is non-zero.
 * @param len Number of SheetElements.
 *
 * @return A non-zero value if sheet[*i] contains a time signature (SET_BARLENGTH) and that time signature was displayed. Otherwise, zero.
 */
int do_time_signature_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;

	e = &sheet[*i];
	if (SET_BARLENGTH == e->type) {
		if (startedNoteOutput) {
			if (lookahead_has_notes(sheet, *i, len)) {
				printf(" \\");
			}
			printf("\n");
		}
		
		if (4 == e->barlength.numerator
				&& 4 == e->barlength.denominator) {
			printf("M:C\n");
		}
		else {
			printf("M:%d/%d\n",
				e->barlength.numerator,
				e->barlength.denominator);
		}

		startedNoteOutput = 0;
		
		*i += 1;
		return 1;
	}

	return 0;
}

/**
 * If sheet[*i] contains a tune header, then prints it, adds 1 to *i, and
 * returns a non-zero value. Otherwise, returns 0.
 *
 * The fields X:, T:, L: and C: are printed. X: is printed using an internal
 * counter, the first header would have X:1 and others will have larger values.
 * T: and C: are printed if title and header fields in the sheet[*i]->header
 * are not NULL.
 *
 * @param sheet List of SheetElements.
 * @param i Pointer to the current index of SheetElements. Will be modified if return value is non-zero.
 * @param len Number of SheetElements.
 *
 * @return A non-zero value if sheet[*i] contains a tune header and that tune header was displayed. Otherwise, zero.
 */
int do_header_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;

	e = &sheet[*i];
	if (SET_HEADER == e->type) {
		if (startedNoteOutput) {
			printf("\n\n");
		}
		printf("X:%d\n", currentTuneNumber);
		
		if (e->header.title) {
			printf("T:%s\n", e->header.title);
		}
		if (e->header.composer) {
			printf("C:%s\n", e->header.composer);
		}
		/* todo: make default length changeable */
		printf("L:1/8\n");
		
		startedNoteOutput = 0;
		currentTuneNumber++;
		
		*i += 1;
		return 1;
	}

	return 0;
}

/**
 * If sheet[*i] contains a note, displays it, advances the index in i,
 * and returns a non-zero value. Otherwise, returns 0.
 *
 * @param sheet List of SheetElements.
 * @param i Pointer to the current index of SheetElements. Will be modified if return value is non-zero.
 * @param len Number of SheetElements.
 *
 * @return A non-zero value if sheet[*i] contains a note and that note was displayed. Otherwise, zero.
 */
int do_note_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;
	
	e = &sheet[*i];	
	if (SET_NOTE == e->type && 0 != e->note.number) {
		if (!keyPrinted) {
			print_key_signature(sheet, i, len);
		}

		print_note(&sheet[*i].note);
		print_note_length(&e->note);
		if (e->note.tied) {
			printf("-");
		}
		if (*i + 1 >= len || SET_SLUR != sheet[*i+1].type
				|| !sheet[*i+1].slur.is_closing) {
			printf(" ");
		}

		*i += 1;
		return 1;
	}

	return 0;
}

/* If sheet[*i] and sheet[*i + 1] contain two notes: one note with N dots,
 * and another note 2^n times smaller, then display these notes as
 * first>...>second or first<...<second with N > or < signs.
 *
 * If the display is successful, advances the pointer
 * in *i by 2, and returns a non-zero value. Otherwise, returns 0.
 *
 * @param sheet List of SheetElements.
 * @param i Pointer to the current index of SheetElements. Will be modified if return value is non-zero.
 * @param len Number of SheetElements.
 *
 * @return A non-zero value if sheet[*i] and sheet[*i + 1] contains two notes that can be displayed using the broken rhythm syntax of ABC. Otherwise, zero.
 * */
int do_broken_rhythm_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e1, *e2;
	Note tmpNote;
	int j;

	if (*i +1 >= len) {
		return 0;
	}
	
	e1 = &sheet[*i];
	e2 = &sheet[*i + 1];
	/* TODO: refactor do_broken_rhythm_display to avoid duplication */
	if (SET_NOTE == e1->type && 0 != e1->note.number
			&& SET_NOTE == e1->type
			&& 0 != e2->note.number) {
		if (!keyPrinted) {
			print_key_signature(sheet, i, len);
		}
		if (0 != e1->note.dots && 0 == e2->note.dots
				&& e1->note.length >> e1->note.dots
					== e2->note.length) {
			tmpNote = e1->note;
			tmpNote.dots = 0;
			
			print_note(&e1->note);
			print_note_length(&tmpNote);
			if (e1->note.tied) {
				printf("-");
			}
			for (j = 0; j < e1->note.dots; j++) {
				printf(">");
			}
			print_note(&e2->note);
			print_note_length(&tmpNote);
			if (e2->note.tied) {
				printf("-");
			}

			printf(" ");
			
			*i += 2;
			return 1;
		}

		if (0 != e2->note.dots && 0 == e1->note.dots
				&& e2->note.length >> e2->note.dots
					== e1->note.length) {
			tmpNote = e2->note;
			tmpNote.dots = 0;
			
			print_note(&e1->note);
			print_note_length(&tmpNote);
			if (e1->note.tied) {
				printf("-");
			}
			for (j = 0; j < e2->note.dots; j++) {
				printf("<");
			}
			print_note(&e2->note);
			print_note_length(&tmpNote);
			if (e2->note.tied) {
				printf("-");
			}

			printf(" ");
			
			*i += 2;
			return 1;
		}
		

		return 0;
	}

	return 0;
}


int do_semibreve_rest_display(SheetElement *sheet, int *i, int len) {
	int j, foundFlag;
	SheetElement *e;
	
	if (*i + 4 <= len) {
		foundFlag = 1;
		
		for (j = 0; j < 4; j++) {
			e = &sheet[*i + j];
			if (SET_NOTE != e->type	|| 0 != e->note.number
					|| LENGTH_CROTCHET != e->note.length
					|| 0 != e->note.dots) {
				foundFlag = 0;
				break;
			}
		}

		if (foundFlag) {
			if (!keyPrinted) {
				print_key_signature(sheet, i, len);
			}
			printf("z%d", LENGTH_SEMIBREVE / currentNoteLength);

			*i += 4;
			return 1;
		}
	}

	return 0;
}

int do_minim_rest_display(SheetElement *sheet, int *i, int len) {
	int j, foundFlag;
	SheetElement *e;
	
	if (*i + 2 <= len) {
		foundFlag = 1;
		
		for (j = 0; j < 2; j++) {
			e = &sheet[*i + j];
			if (SET_NOTE != e->type	|| 0 != e->note.number
					|| LENGTH_CROTCHET != e->note.length
					|| 0 != e->note.dots) {
				foundFlag = 0;
				break;
			}
		}

		if (foundFlag) {
			if (!keyPrinted) {
				print_key_signature(sheet, i, len);
			}
			printf("z%d", LENGTH_MINIM / currentNoteLength);

			*i += 2;
			return 1;
		}
	}

	return 0;
}

int do_rest_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;
	
	e = &sheet[*i];	
	if (SET_NOTE == e->type && 0 == e->note.number) {
		if (!keyPrinted) {
			print_key_signature(sheet, i, len);
		}

		printf("z");
		print_note_length(&e->note);

		*i += 1;
		return 1;
	}

	return 0;
}


int do_newline_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;
	
	e = &sheet[*i];	
	if (SET_NEWLINE == e->type) {
		printf("\n");
		
		*i += 1;
		return 1;
	}

	return 0;
}

int do_bar_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;
	
	e = &sheet[*i];	
	if (SET_BAR == e->type) {
		if (!keyPrinted) {
			print_key_signature(sheet, i, len);
		}

		if (BT_SINGLE == e->bar.type) {
			printf(" |  ");
		}
		else if (BT_DOUBLE == e->bar.type) {
			printf(" ||  ");
		}
		else if (BT_END == e->bar.type) {
			printf(" |]  ");
		}
		else if (BT_REPEAT_BEGIN == e->bar.type) {
			printf(" |:  ");
		}
		else if (BT_REPEAT_END == e->bar.type) {
			printf(" :|  ");
		}
		else if (BT_REPEAT_BOTH == e->bar.type) {
			printf(" ::  ");
		}
		
		*i += 1;
		return 1;
	}

	return 0;
}

int do_slur_display(SheetElement *sheet, int *i, int len) {
	SheetElement *e;
	
	e = &sheet[*i];	
	if (SET_SLUR == e->type) {
		if (!keyPrinted) {
			print_key_signature(sheet, i, len);
		}

		if (e->slur.is_closing) {
			printf(") ");
		}
		else {
			printf("(");
		}
		
		*i += 1;
		return 1;
	}

	return 0;
}

#define SDT_HEADER 1
#define SDT_NOTE_OUTPUT 2
#define SDT_LINE 4

typedef int (*SheetDisplayerFn)(SheetElement *, int *, int);
typedef struct SheetDisplayer {
	SheetDisplayerFn fn;
	int type;
} SheetDisplayer;

SheetDisplayer displayers[] = {
	{do_header_display, SDT_HEADER},
	{do_transposition_display, SDT_LINE},
	{do_time_signature_display, SDT_LINE},

	{do_newline_display, SDT_LINE},
	{do_bar_display, SDT_NOTE_OUTPUT},
	
	{do_semibreve_rest_display, SDT_NOTE_OUTPUT},
	{do_minim_rest_display, SDT_NOTE_OUTPUT},
	{do_rest_display, SDT_NOTE_OUTPUT},

	{do_slur_display, SDT_NOTE_OUTPUT},
	{do_broken_rhythm_display, SDT_NOTE_OUTPUT},
	{do_note_display, SDT_NOTE_OUTPUT}
};

void print_sheet(SheetElement *sheet, int sheetLen) {
	int i, j, processed;
	
	reset_current_accidentals();
	i = 0;
	while (i < sheetLen) {
		processed = 0;
		for (j = 0; j < LENGTH(displayers); j++) {
			if (displayers[j].fn(sheet, &i, sheetLen)) {
				processed = 1;
				
				if (displayers[j].type & SDT_NOTE_OUTPUT) {
					startedNoteOutput = 1;
				}

				break;
			}
		}
		
		if (!processed) {
			printf("??? ");
			i++;
		}
	}
	
	printf("\n");
}
