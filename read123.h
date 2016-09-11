void set_default_accidental(char note_number, signed char semitone_shift);
signed char get_default_accidental(char note_number);
void clean_default_accidentals(void);
void read_note(int accidental, int note_number, int octave_shift, int note_length, int dots, int tied, int separated);
void read_slur(int is_closing);
void read_newline();
void read_bar(BarType bar_type);
void read_bar_length(int numerator, int denominator);
void read_header(char *header);
void read_major_scale_transposition(int note_letter, int semitone_shift);

extern char *headerTitle;
extern char *headerComposer;
