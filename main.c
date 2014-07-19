#include "swipe_read.h"

#define TIMEOUT 2

void fpurge();

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
	
	/* infinite loop of reading, parsing, and checking */
	system("clear");
	for (;;) {
		get_swipe_data(buffer);
		if (!strcmp(buffer, "exit\n")) {
			printf("Exit command received.\n");
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
	free(card);
	free(buffer);
	return 0;
}
