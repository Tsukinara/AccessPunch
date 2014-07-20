#include "swipe_read.h"

/*
 * get_swipe_data: returns the string obtained after swiping a card.
 * Blocks until a card is swiped.
 */

void get_swipe_data(char *buffer) {
	int i = 0; char c;
	__fpurge(stdin); fflush(stdin); __fpurge(stdin); fflush(stdin);
	fprintf(stdout, "Waiting for card swipe...\n");
	
	scanf("%c", &c);
	while (c != '\n') {
		buffer[i] = c;
		i++;
		scanf("%c", &c);
	}
	buffer[i] = '\0';
	/*
	fgets(buffer, BUFFER_LEN, stdin);
	/* replace newline with \0
	buffer[strlen(buffer)] = '\0';
	*/
}

/*
 * reset_card: given the card to be reset, sets all string values to null
 * characters to prevent leftovers from previous writes after strcpy is called.
 */

void reset_card(struct card *c) {
	int i;
	c->type = UNKNOWN;
	c->year = 0; c->month = 0; c-> day = 0;
	for (i = 0; i < BUFFER_LEN; i++) {
		c->name[i] = '\0';
		c->number[i] = '\0';
	}
}

/* 
 * generate_card: given the raw data and a card structure to write to, this
 * method handles how to parse the raw data, then parses the data to the card.
 */

void generate_card(struct card *c, char *data) {
	int start, end, counter = 0;
	if (strlen(data) < 10) {
		reset_card(c);
		return;
	}
	if (strlen(data) < 25) {
		/* get card ID */
		c->type = ID;
		while (! ISINT(data[counter])) {
			counter++;
			if (data[counter] == '\0') { reset_card(c); return; }
		}
		start = counter;
		while (ISINT(data[counter])) {
			if (data[counter] == '\0') { reset_card(c); return; }
			counter++;
		}
		end = counter;
		memcpy(c->number, data + start, end - start);
		if (strlen(c->number) != 15) { reset_card(c); return; };
		/* look up number in database */
		db_lookup(c);
	} else {
		parse_credit(c, data);
	}
}

/*
 * parse_credit: given a card structure to parse the data to and the raw
 * data from the scan, parses the scanned data to human-readable information
 * and writes it to the card structure.
 */

void parse_credit(struct card *c, char *data) {
	int start, end, counter = 0;
	char last[BUFFER_LEN], first[BUFFER_LEN], yr[3], mo[3], dy[3];
	
	/* get credit card number */
	c->type = CREDIT;
	while (! ISINT(data[counter])) {
		counter++;
		if (data[counter] == '\0') { reset_card(c); return; }
	}
	start = counter;
	while (ISINT(data[counter])) {
		counter++;
		if (data[counter] == '\0') { reset_card(c); return; }
	}
	end = counter;
	memcpy(c->number, data + start, end - start);
	
	if (strlen(c->number) != 16) { reset_card(c); return; }
	
	/* get last name */
	while (data[counter] != '^') {
		counter++;
		if (data[counter] == '\0') { reset_card(c); return; }
	}
	start = counter + 1;
	while (data[counter] != '/') {
		counter++;
		if (data[counter] == '\0') { reset_card(c); return; }
	}
	end = counter;
	memcpy(last, data + start, end - start);
	last[end - start] = '\0';
	
	/* get first name */
	start = counter + 1;
	while (data[counter] != '^') {
		counter++;
		if (data[counter] == '\0') { reset_card(c); return; }
	}
	end = counter;
	memcpy(first, data + start, end - start);
	first[end - start] = '\0';	
	/* remove trailing whitespace */
	end = strlen(first) - 1;
	while (! ISTEXT(first[end])) {
		first[end--] = '\0';
		if (data[counter] == '\0') { reset_card(c); return; }
	}
	
	/* set name in card */
	sprintf(c->name, "%s, %s", last, first);
	
	/* get expiration date */
	counter++;
	yr[0] = data[counter++]; yr[1] = data[counter++]; yr[2] = '\0';
	mo[0] = data[counter++]; mo[1] = data[counter++]; mo[2] = '\0';
	dy[0] = data[counter++]; dy[1] = data[counter++]; dy[2] = '\0';
	c->year = atoi(yr) + 2000;
	c->month = atoi(mo);
	c->day = atoi(dy);
}

/*
 * play_sound: given the name of an audio file (MP3 only), the method forks,
 * and the child plays the sound using mpg123. The parent reaps the child, but
 * does not block, in case the audio file is long.
 */

void play_sound(const char *audio_file) {
	pid_t child_pid;
	if ((child_pid = fork()) < 0) {
		fprintf(stderr, "Error: fork error\n");
	}
	if (child_pid) {
		waitpid(child_pid, NULL, WNOHANG);
		return;
	} else {
		execlp("mpg123", "mpg123", "-q", audio_file, NULL);
	}
}

/*
 * print_card: given a card, prints information about the card to the console.
 */
 
void print_card(const struct card *c) {
	printf("\n");
	switch (c->type) {
		case UNKNOWN:
			printf("Card not recognized.\n");
			break;
		case ID:
			printf("Student ID recognized:\n");
			printf("Card number: %s\n", c->number);
			printf("Database entry: %s\n", c->name);
			break;
		case CREDIT:
			printf("Credit/debit card recognized:\n");
			printf("Card number: %s\n", c->number);
			printf("Name on card: %s\n", c->name);
			printf("Expires on: %d/%02d/%02d\n", c->year, c->month, c->day);
			break;
	}
}

/*
 * write_log: given a card, writes the information about the card into
 * history.log, along with a timestamp. This file can then be parsed by other
 * programs to generate nicer looking logs.
 */

void write_log(const struct card *c) {
	FILE *log = fopen("history.log", "a");
	time_t now;
	now = time(NULL);
	fprintf(log, "%s", ctime(&now));
	switch (c->type) {
		case UNKNOWN:
			fprintf(log, "Unrecognized card\n\n");
			break;
		case ID:
			fprintf(log, "Student ID card: %s\n%s\n", c->number, c->name);
			fprintf(log, (check_whitelist(c)?"Access granted\n\n":"Access denied\n\n"));
			break;
		case CREDIT:
			fprintf(log, "Credit card: %s\n%s\n", c->number, c->name);
			fprintf(log, "Expires %d/%02d/%02d\n\n", c->year, c->month, c->day);
			break;
	}
	fclose(log);
}

/*
 * check_whitelist: given a card structure, if the card is an ID card, this
 * method checks to see if the magnetic strip number is in the whitelist. If
 * it is, 1 is returned. 0 is returned in all other cases.
 */

int check_whitelist(const struct card *c) {
	FILE *whitelist = fopen("whitelist.txt", "r");
	char buffer[BUFFER_LEN + 1] = "";
	char number[ID_LEN + 1] = "";
	
	/* only IDs will let you in */
	if (c->type != ID) return 0;
	
	/* search through file */
	fgets(buffer, BUFFER_LEN, whitelist);
	while (!feof(whitelist)) {
		sscanf(buffer, "%s", number);
		if (!strcmp(c->number, number)) {
			fclose(whitelist);
			return 1;
		}
		fgets(buffer, BUFFER_LEN, whitelist);
	}
	
	/* close file and return 0 if not found in whitelist */
	fclose(whitelist);
	return 0;
}

/* 
 * db_lookup: given the card structure, if the card is a student ID card,
 * looks up the magnetic strip number in the database and tries to match
 * it with a name. If it finds a name, it sets the name field of the card,
 * otherwise, it sets it to "No database entry."
 */
 
void db_lookup(struct card *c) {
	FILE *database = fopen("database.txt", "r");
	char buffer[BUFFER_LEN + 1] = "";
	char number[ID_LEN + 1] = "";
	
	/* the database only holds data on IDs */
	if (c->type != ID) return;
	
	/* search through file */
	fgets(buffer, BUFFER_LEN, database);
	while (!feof(database)) {
		/* get number */
		sscanf(buffer, "%s", number);
		
		if (!strcmp(c->number, number)) {
			/* move pointer to associated name */
			fgets(c->name, BUFFER_LEN, database);
			c->name[strlen(c->name) - 1] = '\0';
			fclose(database);
			return;
		} else {
			fgets(buffer, BUFFER_LEN, database);
		}
		
		/* move on to next number if not found */
		fgets(buffer, BUFFER_LEN, database);
	}
	fclose(database);
	
	/* set default name if not found */
	strcpy(c->name, "No database entry.");
}

