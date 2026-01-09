#include "game.h"

int main(void)
{
	game *g = init_game();
	while (g->is_running) {
		handle_input(g);
		update(g);
		render(g);
		SDL_Delay(16);
	}
	cleanup(g);

	return 0;
}
