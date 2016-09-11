#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "musje2abc.h"
#include "read123.h"
#include "writeabc.h"

#define INITIAL_NOTE_LIST_LENGTH 128

SheetElement *sheet;
int sheetLen = 0;
int sheetAllocated = 0;
int barLengthNumerator, barLengthDenominator;
char *currentHeader = NULL;

int clean_note(Note *n) {
	n->number = 0;
	n->semitone_shift = 0;
	n->accidental = DEFAULT_ACCIDENTAL;
	n->octave = 4;
	n->length = LENGTH_CROTCHET;
	n->dots = 0;
}

Note* get_current_note() {
	return &sheet[sheetLen].note;
}

SheetElement *get_current_sheet_element() {
	return &sheet[sheetLen];
}

Note *next_note() {
	sheetLen++;
	if (sheetAllocated <= sheetLen) {
		sheetAllocated *= 2;
		sheet = realloc(sheet, sizeof(SheetElement) * sheetAllocated);
		assert(sheet != NULL);
	}
	sheet[sheetLen].type = SET_NOTE;
	clean_note(&sheet[sheetLen].note);
	
	return &sheet[sheetLen].note;
}

void init_convertor() {
	sheet = malloc(sizeof(SheetElement) * INITIAL_NOTE_LIST_LENGTH);
	assert(sheet != NULL);
	sheetLen = 0;
	sheetAllocated = INITIAL_NOTE_LIST_LENGTH;
	sheet[0].type = SET_NOTE;
	clean_note(get_current_note());
	clean_default_accidentals();
	
	set_current_key(NOTE_C, 0);
	/* set_current_key(NOTE_G, 1); */
}

void display_result(void) {
	print_sheet(sheet, sheetLen);
}

