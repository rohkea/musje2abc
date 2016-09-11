Note* get_current_note();
SheetElement *get_current_sheet_element();
Note *next_note();
void init_convertor();
void display_result(void);

/* TODO: refactor bar length reading not to use global variables */
extern int barLengthNumerator, barLengthDenominator;
extern char *currentHeader;
