#define ABC_MIDDLE_OCTAVE 4

int get_key_singature_semitone_shift(int note_key);
int set_current_key(int key, int semitone_shift);
void print_note(Note *n);
void print_current_key(void);
void print_sheet(SheetElement *sheet, int sheetLen);
