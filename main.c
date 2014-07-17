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
			/*switch (card->type) {
			case ID:
				if (check_whitelist(card)) {
					play_sound("success.mp3");
				} else {
					play_sound("failure.mp3");
				}
				break;
			case CREDIT:
				play_sound("retard.mp3");
				break;
			case UNKNOWN:
				play_sound("unrecognized.mp3");
				break;
			}
			write_log(card);*/
			free(buffer);
			free(card);
			card = malloc(sizeof(Card));
		}
		sleep(TIMEOUT);
		system("clear");
	}
	return 0;
}
