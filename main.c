#include "swipe_read.h"

int main(int argc, char *argv[]) {
	char *buffer;
	Card *card = malloc(sizeof(Card));

	if (!card) {
		fprintf(stderr, "Error: memory allocation failed\n");
		return -1;
	}

	for (;;) {
		buffer = get_swipe_data();
		if (!strcmp(buffer, "exit\n")) {
			printf("Exit command received.\n");
			return 0;
		} else {
			generate_card(card, buffer);
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
		/*system("clear");*/
	}
	return 0;
}
