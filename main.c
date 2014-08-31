#include "swipe_read.h"

#define TIMEOUT 2
#define IDLE 2
#define GRNT 3
#define DENY 4
#define MONY 5

void write_pins(int i, int g, int d, int m);
void GPIO_setup();
void set_lights(char mode);

void digital_write(int pin, int mode);
int swipe_loop();

/*
 * main: creates a card object, and infinitely loops through, reading data
 * to the card object, and then parsing that object.
 */

int main(int argc, char *argv[]) {
	/* set up GPIO */
	GPIO_setup();

	/* get most recent files before running */
	system("clear");
	printf("Please wait\n");
	printf("Retrieving most recent files from GitHub...\n");
	fflush(stdout);
	system("git pull");
	sleep(TIMEOUT);

	/* clear screen and begin loop */
	system("clear");
	return swipe_loop();
}

int swipe_loop() {
	char *buffer = malloc(BUFFER_LEN + 1);
	Card *card = malloc(sizeof(Card));

	/* check for malloc failure */
	if (!card) {
		fprintf(stderr, "Error: memory allocation failed\n");
		return -1;
	}

	/* infinite loop of reading, parsing, and checking */
	for (;;) {
		get_swipe_data(buffer);
		if (!strcmp(buffer, "exit")) {
			printf("Exit command received.\n");
			free(card);
			free(buffer);

			printf("Copying log to rock (hitsuji down for maintenance)\n");
			system("scp history.log mattdu@rock.umiacs.umd.edu:~/Documents/");
			return 0;
		} else {
			reset_card(card);
			generate_card(card, buffer);
			print_card(card);
			switch (card->type) {
			case ID:
				if (check_whitelist(card)) {
					printf("\n\nAccess granted.\n");
					play_sound("success.mp3");
					set_lights('g');
				} else {
					printf("\n\nAccess denied.\n");
					play_sound("failure.mp3");
					set_lights('d');
				}
				break;
			case CREDIT:
				printf("\n\nWe thank you for your donation.\n");
				play_sound("donation.mp3");
				set_lights('m');
				break;
			case UNKNOWN:
				printf("\n\nSorry, card not recognized.\n");
				play_sound("unrecognized.mp3");
				set_lights('d');
				break;
			}
			write_log(card);
		}

		/* wait a bit so we can see any output to console */
		sleep(TIMEOUT);
		set_lights('i');
		system("clear");
	}
	return 0;
}

void GPIO_setup() {
	char *buffer = malloc(BUFFER_LEN);
	sprintf(buffer, "gpio mode %d OUTPUT", IDLE);
	system(buffer);
	sprintf(buffer, "gpio mode %d OUTPUT", GRNT);
	system(buffer);
	sprintf(buffer, "gpio mode %d OUTPUT", DENY);
	system(buffer);
	sprintf(buffer, "gpio mode %d OUTPUT", MONY);
	system(buffer); free(buffer);

	/* set lights to idle mode */
	write_pins(1, 0, 0, 0);
}

void set_lights(char mode) {
	switch (mode) {
		case 'i':
			write_pins(1, 0, 0, 0);
			break;
		case 'g':
			write_pins(0, 1, 0, 0);
			break;
		case 'd':
			write_pins(0, 0, 1, 0);
			break;
		case 'm':
			write_pins(0, 0, 0, 1);
			break;
	}
}

void write_pins(int i, int g, int d, int m) {
	if (i == 1) digital_write(IDLE, i);
	if (g == 1) digital_write(GRNT, g);
	if (d == 1) digital_write(DENY, d);
	if (m == 1) digital_write(MONY, m);

	digital_write(IDLE, i);
	digital_write(GRNT, g);
	digital_write(DENY, d);
	digital_write(MONY, m);
	return;
}

void digital_write(int pin, int mode) {
	char *buffer = malloc(BUFFER_LEN);
	sprintf(buffer, "gpio write %d %d", pin, mode);
	system(buffer); free(buffer);
}
