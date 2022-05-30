
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <time.h>


SDL_Window *window = NULL;
SDL_Surface *surface = NULL;



#define BG_COL 0x232b2bff
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int init(void) 
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initialized. please try again; error: %s\n", SDL_GetError());
	} else {
	        window = SDL_CreateWindow("pong", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_WIDTH, 
			SCREEN_HEIGHT, 
			SDL_WINDOW_SHOWN 
			);

		if (!window) {
			printf("window could not be initialized. please try again; error %s\n", SDL_GetError());
		} else {
			surface = SDL_GetWindowSurface(window);
			return 0;
		}
	}

	return 1;


}




void cleanup(void)
{
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
}



typedef struct {
	SDL_Rect r;
	float radian;
} ball_t;

typedef struct {
	SDL_Rect r;
	float a;
} pad_t;


SDL_Rect player_border = {
	.x = 0,
	.y = 0, 
	.w = 10,
	.h = SCREEN_HEIGHT
};


SDL_Rect computer_border = {
	.x = SCREEN_WIDTH - 10,
	.y = 0,
	.w = 10,
	.h = SCREEN_HEIGHT

};


pad_t computer = {
	.r = {
		.x = SCREEN_WIDTH - 20,
		.y = 10,
		.h = 70,
		.w = 10
	},
	.a = 0.0
};

pad_t player = {
	.r = {
		.x = 10,
		.y = 10,
		.h = 70,
		.w = 10
	},
	.a = 0.0
};

ball_t ball = {
	.r = {
		.x = SCREEN_WIDTH / 2,
		.y = SCREEN_HEIGHT / 2,
		.h = 10,
		.w = 10,
	},
	.radian = 0.3
};

const double t = 1.f / 60.f;

int player_score;
int computer_score;
float xdir = -1.f; 
float ydir = 1.f;



void reset_ball(ball_t *r) 
{
	r->r.x = SCREEN_WIDTH / 2;
	r->r.y = SCREEN_HEIGHT / 2;
	xdir *= -1.f;

}


void move_ball(ball_t *r) 
{
	
	double y = (double) r->r.y;
	double x = (double) r->r.x;
	
	y += t * tanl(r->radian) * SCREEN_HEIGHT * ydir; 
	x += t * SCREEN_WIDTH / 4 * xdir;

	r->r.y = (int) y;
	r->r.x = (int) x;

	if (SDL_HasIntersection((SDL_Rect *) r, &computer_border)) {
		player_score++;
		reset_ball(r);
	} else if (SDL_HasIntersection((SDL_Rect *) r, &player_border)) {
		computer_score++;
		reset_ball(r);
	}

	if (SDL_HasIntersection((SDL_Rect *) r, (SDL_Rect *) &computer)) {
		ball.radian = (double)rand() / (double)RAND_MAX / 2;
		xdir *= -1.f;
		r->r.x = SCREEN_WIDTH - 30;

	} else if (SDL_HasIntersection((SDL_Rect *) r, (SDL_Rect *) &player)) {
		ball.radian = (double)rand() / (double)RAND_MAX / 2;
		r->r.x = 20;
		xdir *= -1.f;
	}

	if (r->r.y < 0) {
		r->r.y = 0;
		ydir *= -1.f;
	} else if (r->r.y > (SCREEN_HEIGHT - r->r.h)) {
		r->r.y = SCREEN_HEIGHT - r->r.h;
		ydir *= -1.f;
	}




	
}
int point = -1;

void move_paddle(pad_t *r)
{

	double y = (double) r->r.y;
	y += t * r->a;
	

	r->r.y = (int) y;
	if (r->r.y < 0) {
		r->r.y = 0;
	} else if (r->r.y > (SCREEN_HEIGHT - r->r.h)) {
		r->r.y = SCREEN_HEIGHT - r->r.h;
	}

}


void move_computer(pad_t *r)
{
	if (r->r.y >= ball.r.y) {
		r->a = (double) -0.247 * (double) SCREEN_HEIGHT; 
	} else {
		r->a = (double) 0.247 * (double) SCREEN_HEIGHT; 
	}
	move_paddle(r);


}




int main(int argc, char *argv[]) 
{


	srandom(time(NULL));
	SDL_Event event;
	
	if (init()) {
		printf("failure.\n");
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	int gameover = 0;
	while(!gameover) {
		SDL_SetRenderDrawColor(renderer, 0x23, 0x2b, 0x2b, 0xff);
		SDL_RenderClear(renderer);



		SDL_SetRenderDrawColor(renderer, 0x00, 0x66, 0xc5, 0xff);
		SDL_RenderDrawRect(renderer, (SDL_Rect *) &player);

		SDL_SetRenderDrawColor(renderer, 0xb8, 0x0f, 0x0a, 0xff);
		SDL_RenderDrawRect(renderer, (SDL_Rect *) &computer);

		SDL_SetRenderDrawColor(renderer, 0xff, 0x66, 0xc5, 0xff);
		SDL_RenderDrawRect(renderer, (SDL_Rect *) &ball);




		move_paddle(&player);
		move_computer(&computer);
		move_ball(&ball);


		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameover += 1;
			} else if (event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
				case SDLK_j:
					player.a = (double) 1.0 * (double) SCREEN_HEIGHT; 
					break;
				case SDLK_k:
					player.a = (double) -1.0 * (double)  SCREEN_HEIGHT;
				}
				
			} else if (event.type == SDL_KEYUP) {
				player.a = 0;
			}


		}








		SDL_RenderPresent(renderer);
	}

	cleanup();

	
	return 0;




}



