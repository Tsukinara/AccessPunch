#include "swipe_read.h"

#define TIMEOUT 2

int main(int argc, char *argv[]) {
	char *buffer;
	Card *card = malloc(sizeof(Card));

	if (!card) {
		fprintf(stderr, "Error: memory allocation failed\n");
		return -1;
	}

	system("clear");
	for (;;) {
		buffer = get_swipe_data();
		if (!strcmp(buffer, "exit\n")) {
			printf("Exit command received.\n");
			return 0;
		} else {
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
				play_sound("retard.mp3");
				break;
			case UNKNOWN:
				printf("\n\nSorry, card not recognized.\n");
				play_sound("unrecognized.mp3");
				break;
			}
			/* write_log(card); */
			free(buffer);
			reset_card(card);
		}
		sleep(TIMEOUT);
		system("clear");
	}
	return 0;
}
