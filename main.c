#include "swipe_read.h"

#define TIMEOUT 2

/*
 * main: creates a card object, and infinitely loops through, reading data
 * to the card object, and then parsing that object.
 */

int main(int argc, char *argv[]) {
	char *buffer = malloc(BUFFER_LEN + 1);
	Card *card = malloc(sizeof(Card));

	/* check for malloc failure */
	if (!card) {
		fprintf(stderr, "Error: memory allocation failed\n");
		return -1;
	}

	/* get most recent files before running */
	system("clear");
	printf("Please wait\n");
	printf("Retrieving most recent files from GitHub...\n");
	fflush(stdout);
	system("git pull");
	sleep(TIMEOUT);

	/* infinite loop of reading, parsing, and checking */
	system("clear");

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
				} else {
					printf("\n\nAccess denied.\n");
					play_sound("failure.mp3");
				}
				break;
			case CREDIT:
				printf("\n\nWe thank you for your donation.\n");
				play_sound("donation.mp3");
				break;
			case UNKNOWN:
				printf("\n\nSorry, card not recognized.\n");
				play_sound("unrecognized.mp3");
				break;
			}
			write_log(card);
		}

		/* wait a bit so we can see any output to console */
		sleep(TIMEOUT);
		system("clear");
	}

	return 0;
}
