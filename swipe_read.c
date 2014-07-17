#include "swipe_read.h"

char *get_swipe_data() {
	char buffer[BUFFER_LEN + 1], *pointer;
	int len;
	fprintf(stdout, "Waiting for card swipe...\n");
	fgets(buffer, BUFFER_LEN, stdin);
	
	len = strlen(buffer);
	
	/* allocate len bytes because newline character gets replaced with \0 */
	pointer = malloc(len);
	if (!pointer) {
		fprintf(stderr, "Error: memory allocation failed\n");
		exit(0);
	}
	
	memcpy(pointer, buffer, len);
	
	/* replace newline with \0 */
	pointer[len] = '\0';
	return pointer;
}

void reset_card(struct card *c) {
	int i;
	for (i = 0; i < BUFFER_LEN; i++) {
		c->name[i] = '\0';
		c->number[i] = '\0';
	}
}

void generate_card(struct card *c, char *data) {
	int start, end, counter = 0;
	if (strlen(data) < 10) {
		c->type = UNKNOWN;
		return;
	}
	if (strlen(data) < 25) {
		/* get card ID */
		c->type = ID;
		while (! ISINT(data[counter])) counter++;
		start = counter;
		while (ISINT(data[counter])) counter++;
		end = counter;
		memcpy(c->number, data + start, end - start);
		
		/* look up number in database */
		db_lookup(c);
	} else {
		parse_credit(c, data);
	}
}

void parse_credit(struct card *c, char *data) {
	int start, end, counter = 0;
	char last[BUFFER_LEN], first[BUFFER_LEN], yr[3], mo[3], dy[3];
	
	/* get credit card number */
	c->type = CREDIT;
	while (! ISINT(data[counter])) counter++;
	start = counter;
	while (ISINT(data[counter])) counter++;
	end = counter;
	memcpy(c->number, data + start, end - start);
	c->number[end - start] = '\0';
	
	/* get last name */
	while (data[counter] != 94) counter++;
	start = counter + 1;
	while (data[counter] != 47) counter++;
	end = counter;
	memcpy(last, data + start, end - start);
	last[end - start] = '\0';
	
	/* get first name */
	start = counter + 1;
	while (data[counter] != 94) counter++;
	end = counter;
	memcpy(first, data + start, end - start);
	first[end - start] = '\0';
	
	/* remove trailing whitespace */
	end = strlen(first) - 1;
	while (! ISTEXT(first[end])) first[end--] = '\0';
	
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

void write_log(const struct card *c) {

}

int check_whitelist(const struct card *c) {
	FILE *whitelist = fopen("whitelist.txt", "r");
	char buffer[BUFFER_LEN + 1];
	char number[ID_LEN + 1];
	int flag = 0, i = 0;
	
	if (c->type != ID) return 0;
	
	fgets(buffer, BUFFER_LEN, whitelist);
	while (!feof(whitelist)) {
		sscanf(buffer, "%s", number);
		if (!strcmp(c->number, number)) flag = 1;
		fgets(buffer, BUFFER_LEN, whitelist); i++;
	}
	fclose(whitelist);
	return flag;
}

void db_lookup(struct card *c) {
	strcpy(c->name, "No database entry.");
}

