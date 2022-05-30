
#include <math.h>
#include <SDL2/SDL_ttf.h>
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



typedef struct {
	SDL_Rect r;
	double a;
} bird_t;

bird_t bird = {
	.r = {
		.x = 50, 
		.y = 50,
		.w = 20,
		.h = 20
	},
	.a = 5.f
};



SDL_Rect column = {
	.x = SCREEN_WIDTH,
	.y = SCREEN_HEIGHT,
	.w = 100,
	.h = 100 
};
SDL_Rect pillar = {
	.x = SCREEN_WIDTH,
	.y = SCREEN_HEIGHT,
	.w = 100,
	.h = 100 
};


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

int score = 0;




	int gameover = 0;

void cleanup(void)
{
	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
}


const double grav = 3.4;
const double t = 1.f / 60.f;

int mv_bird(bird_t *r) 
{
	double y = r->r.y; 
	y += r->a * t;

	if (r->a < (8 * SCREEN_HEIGHT)) {
		r->a += (SCREEN_HEIGHT + 440)/ 2 * t;
	}

		
	r->r.y = (int) y;


	if (r->r.y < 0 || r->r.y > (SCREEN_HEIGHT - r->r.h) || SDL_HasIntersection(&column, (SDL_Rect *) r) || SDL_HasIntersection(&pillar, 
				(SDL_Rect *) r)) {
		gameover = 1;

	}
}





static inline int cheight() 
{
	return (random() % 130) + 70;
}

void reset_column(SDL_Rect *r)
{
	int ch = cheight();
	r->x = SCREEN_WIDTH + 70;
	r->y = SCREEN_HEIGHT - ch;
	r->w = 70;
	r->h = ch;

}
void reset_pillar(SDL_Rect *r)
{
	int ch = cheight();
	r->x = SCREEN_WIDTH + 70;
	r->y = 0;
	r->w = 70;
	r->h = ch;

}

	
void mv_pillar (SDL_Rect *r)
{
	if (r->x < -70) {
		score++;
		reset_pillar(r);
	}
	double x = r->x;
	x -= SCREEN_WIDTH * t;
	r->x = (int) x;

}

void mv_column (SDL_Rect *r)
{
	if (r->x < -70) {
		score++;
		reset_column(r);
	}
	r->x -= SCREEN_WIDTH / 2 * t;

}

int main(int argc, char *argv[]) 
{
	srandom(time(NULL));
	SDL_Event event;

	
	if (init()) {
		printf("failure.\n");
		return 1;
	}
	TTF_Init();

	TTF_Font *font = TTF_OpenFont("sans.ttf", 200);
	if (!font) {
		printf("font not found");
		return 1;
	}

	char msg[7];
	SDL_Color tcol = {255, 255, 255};

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);




	while(!gameover) {


		SDL_SetRenderDrawColor(renderer, 0x23, 0x2b, 0x2b, 0xff);
		SDL_RenderClear(renderer);

		sprintf(msg, "%d", score);
		SDL_Surface *text = TTF_RenderText_Solid(font, msg, tcol);

		SDL_Rect trect = {
			.x = SCREEN_WIDTH / 2,
			.y = SCREEN_HEIGHT / 2,
			.h = 200,
			.w = 100 * strlen(msg)
		};

		SDL_Texture *mst = SDL_CreateTextureFromSurface(renderer, text);
		SDL_RenderCopy(renderer, mst, NULL, &trect);
		SDL_FreeSurface(text);
		SDL_DestroyTexture(mst);

		SDL_SetRenderDrawColor(renderer, 0xff, 0xfb, 0x2b, 0xff);
		SDL_RenderDrawRect(renderer, (SDL_Rect *) &bird);

		SDL_SetRenderDrawColor(renderer, 0xff, 0xfb, 0x2b, 0xff);
		SDL_RenderDrawRect(renderer, &column);

		SDL_SetRenderDrawColor(renderer, 0xff, 0xfb, 0x2b, 0xff);
		SDL_RenderDrawRect(renderer, &pillar);

		mv_bird(&bird);
		mv_column(&column);
		mv_pillar(&pillar);











		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameover += 1;
			} else if (event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
				case SDLK_SPACE:
					bird.a = -150.f;
					break;
				}	
				
			} else if (event.type == SDL_KEYUP) {

			}


		}








		SDL_RenderPresent(renderer);
	}

	cleanup();

	
	return 0;




}



