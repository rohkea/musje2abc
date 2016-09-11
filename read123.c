#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "common.h"
#include "read123.h"

signed char defaultAccidentals[8];
char *headerTitle = NULL, *headerComposer = NULL;

void set_default_accidental(char note_number, signed char semitone_shift) {
	assert(note_number >= 0 && note_number <= 7);
	defaultAccidentals[note_number] = semitone_shift;
}

signed char get_default_accidental(char note_number) {
	assert(note_number >= 0 && note_number <= 7);
	if (note_number == 0) {
		return 0;
	}
	else {
		return defaultAccidentals[note_number];
	}
}

void clean_default_accidentals() {
	int i;
	
	for (i = 0; i < 7; i++) {
		defaultAccidentals[i] = 0;
	}
}

/* TODO: move the note-writing functions into a separate file, and import it */
Note* get_current_note();
Note *next_note();
SheetElement *get_current_sheet_element();

void read_major_scale_transposition(int note_letter, int semitone_shift) {
	SheetElement *el;

	el = get_current_sheet_element();
	el->type = SET_TRANSPOSITION;
	el->transposition.note_letter = note_letter;
	el->transposition.semitone_shift = semitone_shift;
	next_note();
}


void read_newline() {
	SheetElement *el;

	el = get_current_sheet_element();
	el->type = SET_NEWLINE;
	next_note();
}

void read_bar(BarType bar_type) {
	SheetElement *el;

	el = get_current_sheet_element();
	el->type = SET_BAR;
	el->bar.type = bar_type;
	next_note();
}

void read_slur(int is_closing) {
	SheetElement *el;

	el = get_current_sheet_element();
	el->type = SET_SLUR;
	el->slur.is_closing = is_closing;
	next_note();
}

void read_bar_length(int numerator, int denominator) {
	SheetElement *el;

	el = get_current_sheet_element();
	el->type = SET_BARLENGTH;
	el->barlength.numerator = numerator;
	el->barlength.denominator = denominator;
	next_note();
}

void parse_header(char *header) {
	/* TODO: refactor header parsing */
	int i, len, startTitle, endTitle, startComposer, endComposer, partLen;
	
	/* if we have previous headerTitle or headerComposer data,
	 * free it to avoid memory leaks */
	if (NULL != headerTitle) {
		free(headerTitle);
		headerTitle = NULL;
	}
	if (NULL != headerComposer) {
		free(headerComposer);
		headerComposer = NULL;
	}

	len = strlen(header);

	i = 0;
	while (i < len -1 && !(header[i] == '<' && header[i+1] == '<')) {
		i++;
	}
	if (i >= len - 1) {
		return;
	}

	startTitle = i + 2;
	i += 2;
	while (i < len -1 && !(header[i] == '>' && header[i+1] == '>')) {
		i++;
	}
	if (i >= len -1) {
		return;
	}

	endTitle = i;
	partLen = endTitle - startTitle;
	headerTitle = malloc(partLen + 1);
	strncpy(headerTitle, &header[startTitle], partLen);
	headerTitle[partLen] = '\0';

	i += 2;
	while (i < len && isspace(header[i])) {
		i++;
	}
	if (i >= len) {
		return;
	}

	startComposer = i;
	while (i < len && '\n' != header[i] && '\r' != header[i]) {
		i++;
	}
	endComposer = i - 1;
	while (isspace(header[endComposer])) {
		endComposer--;
	}
	endComposer++;

	if (endComposer <= startComposer) {
		return;
	}

	partLen = endComposer - startComposer;
	headerComposer = malloc(partLen + 1);
	strncpy(headerComposer, &header[startComposer], partLen);
	headerComposer[partLen] = '\0';
}

void read_header(char *header) {
	SheetElement *el;

	el = get_current_sheet_element();
	parse_header(header);
	el->type = SET_HEADER;
	el->header.composer = headerComposer;
	el->header.title = headerTitle;
	
	/* We don't want them to be free'd twice. */
	headerComposer = NULL;
	headerTitle = NULL;

	next_note();
}

void read_note(int accidental, int note_number, int octave_shift, int note_length, int dots, int tied, int separated) {
	Note *note;
	int semitone_shift;
	
	if (accidental != DEFAULT_ACCIDENTAL) {
		semitone_shift = accidental;
		set_default_accidental(note_number, accidental);
	}
	else {
		semitone_shift = get_default_accidental(note_number);
	}
	
	/* Actually, musje only supports 1---. and not 1----
	 * So probably it makes sense to restrict lengtheners to
	 * 1-, 1-- and 1--- (and disallow arbitrary number of dashes)
	 * for compatibility with musje */
	while (note_length == 5 * LENGTH_CROTCHET
			|| note_length > 7 * LENGTH_CROTCHET) {
		/* Fill tied semibreves if neccessary */
		note = get_current_note();
		note->number = note_number;
		note->accidental = accidental;
		note->semitone_shift = semitone_shift;
		note->tied = 1;
		note->length = LENGTH_MINIM;
		note->dots = dots;
		note->octave = DEFAULT_OCTAVE + octave_shift;
		accidental = DEFAULT_ACCIDENTAL;
		
		note = next_note();
		note_length -= LENGTH_SEMIBREVE;
	}

	if (note_length == 3 * LENGTH_CROTCHET) {
		note_length = LENGTH_MINIM;
		dots++;
	}
	if (note_length == 3 * LENGTH_MINIM) {
		note_length = LENGTH_SEMIBREVE;
		dots++;
	}
	if (note_length == 7 * LENGTH_CROTCHET) {
		note_length = LENGTH_SEMIQUAVER;
		dots += 2;
	}
	
	note = get_current_note();
	note->number = note_number;
	note->accidental = accidental;
	note->semitone_shift = semitone_shift;
	note->length = note_length;
	note->dots = dots;
	note->tied = tied;
	note->octave = DEFAULT_OCTAVE + octave_shift;
	note = next_note();
}
