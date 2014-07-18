#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <sysexits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#ifndef SWIPE_READ_H

#ifndef BUFFER_LEN
#define BUFFER_LEN 255
#endif
#ifndef ID_LEN
#define ID_LEN 16
#endif

/* macro to test if x is an integer */
#define ISINT(x) ((x > 47) && (x < 58))
#define ISTEXT(x) (((x > 64) && (x < 91)) || ((x > 96) && (x < 123)))

/* structures */
typedef struct card {
	enum { UNKNOWN = 0, CREDIT, ID } type;
	char name[BUFFER_LEN + 1];
	char number[BUFFER_LEN + 1];
	int year, month, day;
} Card;

/* swipe method prototypes */
char *get_swipe_data();
void play_sound(const char *audio_file);

void generate_card(struct card *c, char *data);
void parse_credit(struct card *c, char *data);

void write_log(const struct card *c);
void reset_card(struct card *c);
void db_lookup(struct card *c);
void print_card(const struct card *c);
int check_whitelist(const struct card *c);

#define SWIPE_READ_H 1
#endif



