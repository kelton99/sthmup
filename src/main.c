
#include "game.h"

static void cap_frame_rate(long *last_frame_time);

int main(void)
{
	game *g = init_game();
	while (g->is_running) {
	    long last_frame_time = SDL_GetTicks();
		handle_input(g);
		update(g);
		render(g);
		cap_frame_rate(&last_frame_time);
	}

	cleanup(g);
	return 0;
}

static void cap_frame_rate(long *last_frame_time)
{
    long expected_frame_time = 1000 / FPS;
    long delta_time = SDL_GetTicks() - *last_frame_time;

    if(expected_frame_time >= delta_time) {
        expected_frame_time -= delta_time;
        SDL_Delay(expected_frame_time);
    }
}
