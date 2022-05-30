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





int gameover = 0;



typedef struct obstacle {
	SDL_Rect r;
	struct {
		unsigned present : 1;
	};
	struct obstacle *next;
	struct obstacle *prev;
} obstacle_t;
obstacle_t oroot;

typedef struct {
	SDL_Rect r;
	double xvel;
} paddle_t;

typedef struct {
	SDL_Rect r;
	double radian;
} ball_t;

paddle_t paddle = {
	.r = {
		.y = SCREEN_HEIGHT - 60,
		.x = SCREEN_WIDTH / 2,
		.w = 100,
		.h = 20,
	},
	.xvel = 0.0

};


ball_t ball = {
	.r = {
		.y = SCREEN_HEIGHT / 2 + 150,
		.x = SCREEN_WIDTH / 2,
		.w = 20,
		.h = 20
	},
	.radian = 1.2


};

double t = 1.f / 60.f;

int mv_paddle(paddle_t *r)
{
	r->r.x += r->xvel * t / 2;
	if (r->r.x < 0) {
		r->r.x = 0;
	} else if (r->r.x > (SCREEN_WIDTH - r->r.w)) {
		r->r.x = SCREEN_WIDTH - r->r.w;
	}
	return 0;
}



void init_obstacle(int rows, int columns, int sx, int sy, int ex, int ey) 
{
	int x = 0, y = 0;
	int spx = (ex - sx) / columns;
	int spy = (ey - sy) / rows;
	obstacle_t *t = &oroot;
	
	for (int i = 0; i < rows; i++) {
		x = sx;
		for (int j = 0; j < columns; j++) {
			t->r.x = x;
			t->r.y = y;
			t->r.w = spx;
			t->r.h = spy;

			t->present = 1;
			x += spx;

			t->next = malloc(sizeof(obstacle_t));
			t = t->next;
		}
		y += spy;
	}

};

double xdir = 1.f;
double ydir = -1.f;

void cleanup(void)
{
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
	obstacle_t *t = &oroot;
	while(t->next) {
		t = t->next;
	}
	while(t != &oroot) {
		t = t->prev;
		free(t->next);
	}
}

int has_coll (ball_t *r)
{
	obstacle_t *t = &oroot;
	while(t) {
		if (t->present) {
			if (SDL_HasIntersection((SDL_Rect *) t, (SDL_Rect *) r)) {
				t->present = 0;
				ydir *= -1.f;
				break;

			}
		}
		t = t->next;
	}
}

int mv_ball(ball_t *r) 
{
	r->r.x += t * SCREEN_WIDTH / 4 * xdir;	
	r->r.y += t * SCREEN_WIDTH / 8 * tanl(r->radian) * ydir;

	if (SDL_HasIntersection((SDL_Rect *) r, (SDL_Rect *) &paddle)) {
		ydir *= -1.f;
	}

	if (r->r.x < 0) {
		r->r.x = 0; 
		xdir *= -1.f;
	} else if (r->r.x > (SCREEN_WIDTH - r->r.w)) {
		r->r.x = SCREEN_WIDTH - r->r.w;
		xdir *= -1.f;
	}

	if (r->r.y < 0) {
		r->r.y = 0; 
		ydir *= -1.f;
	} else if (r->r.y > (SCREEN_HEIGHT - r->r.h)) {
		gameover = 1;
	}

	has_coll(r);
}


void render_o(SDL_Renderer *renderer)
{
	obstacle_t *t = &oroot;
	while (t) {
		if (t->present) {
			SDL_SetRenderDrawColor(renderer, 0x1f, 0xff, 0xff, 0xff);
			SDL_RenderDrawRect(renderer, (SDL_Rect *) t);
		}
		t = t->next;
	}

}

int present() 
{
	int result = 0;
	obstacle_t *t = &oroot;
	while(t) {
		result += t->present;
		t = t->next;
	} 
	return result;
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

	init_obstacle(7, 9, 30, 30, SCREEN_WIDTH - 30, SCREEN_HEIGHT / 2);

	while(!gameover) {

		SDL_SetRenderDrawColor(renderer, 0x23, 0x2b, 0x2b, 0xff);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0xff, 0xfb, 0x2b, 0xff);
		SDL_RenderDrawRect(renderer, (SDL_Rect *) &paddle);

		SDL_SetRenderDrawColor(renderer, 0x1f, 0x80, 0x2b, 0xff);
		SDL_RenderDrawRect(renderer, (SDL_Rect *) &ball);

		render_o(renderer);

		if(!present()) {
			gameover = 1;
		}



		mv_paddle(&paddle);
		mv_ball(&ball);

		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameover += 1;
			} else if (event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
				case SDLK_h:
					paddle.xvel = -SCREEN_WIDTH;
					break;
				case SDLK_l: 
					paddle.xvel = SCREEN_WIDTH;
					break;
				}	
				

			} else if (event.type == SDL_KEYUP) {
				paddle.xvel = 0.f;

			}


		}








		SDL_RenderPresent(renderer);
	}

	cleanup();

	
	return 0;




}



