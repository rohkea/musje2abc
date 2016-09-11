#define LENGTH_SEMIBREVE 512
#define LENGTH_MINIM (LENGTH_SEMIBREVE / 2)
#define LENGTH_CROTCHET (LENGTH_SEMIBREVE / 4)
#define LENGTH_QUAVER (LENGTH_SEMIBREVE / 8)
#define LENGTH_SEMIQUAVER (LENGTH_SEMIBREVE / 16)

#define DEFAULT_ACCIDENTAL 127
#define DEFAULT_OCTAVE 4

#define NOTE_C 0
#define NOTE_D 1
#define NOTE_E 2
#define NOTE_F 3
#define NOTE_G 4
#define NOTE_A 5
#define NOTE_B 6

#define LENGTH(x) ((sizeof x) / (sizeof x[0]))

typedef struct Note {
	signed char number; /* 1 to 7 */
	int length; 
	char tied;
	signed char semitone_shift; /* sharp is 1, flat is -1 */
	signed char accidental;
	char octave; /* 4 is middle C */
	char dots;
} Note;

typedef enum BarType {
	BT_SINGLE, BT_DOUBLE, BT_END,
	BT_REPEAT_BEGIN, BT_REPEAT_END, BT_REPEAT_BOTH
} BarType;

typedef struct Bar {
	BarType type;
} Bar;

typedef struct TuneHeader {
	/* Elements of TuneHeader are allocated with malloc during parsing
	 * and expected to be free'd when they are no longer needed. */
	char *title;
	char *composer;
} TuneHeader;

typedef struct BarLength {
	int numerator, denominator;
} BarLength;

typedef struct Slur {
	int is_closing;
} Slur;

typedef struct Transposition {
	int note_letter;
	int semitone_shift;
} Transposition;

typedef enum SheetElementType {
	SET_NOTE, SET_TRANSPOSITION, SET_BAR, SET_BARLENGTH, SET_HEADER,
	SET_SLUR, SET_NEWLINE
} SheetElementType;

typedef struct SheetElement {
	SheetElementType type;
	union {
		Note note;
		Bar bar;
		BarLength barlength;
		TuneHeader header;
		Transposition transposition;
		Slur slur;
	};
} SheetElement;


