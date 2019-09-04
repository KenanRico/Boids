#include "resources.h"


#include <SDL2/SDL.h>


#define RUNNING 0
#define GAME_SDL_INIT_FAILED 1
#define GAME_SDL_WINDOW_CREATE_FAILED 2
#define GAME_SDL_RENDERER_CREATE_FAILED 3
#define OVER 4


Resources::Resources():
renderer(nullptr),
window(nullptr),
windowW(800), windowH(600),
current_time(0),
FPS(60),
frame(0),
world_width(100.0f), world_height(100.0f),
state(RUNNING)
{
	/*init SDL*/
	if(SDL_Init(SDL_INIT_EVERYTHING)!=0){
		state = GAME_SDL_INIT_FAILED;
		return;
	}
	/*create window*/
	if((window=SDL_CreateWindow("Boids", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowW, windowH, SDL_WINDOW_RESIZABLE))==nullptr){
		state = GAME_SDL_WINDOW_CREATE_FAILED;
		return;
	}
	SDL_HideWindow(window);
	/*create renderer*/
	if((renderer=SDL_CreateRenderer(window, -1, 0))==nullptr){
		state = GAME_SDL_RENDERER_CREATE_FAILED;
	}
}

Resources::~Resources(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Resources::CreateWindow(){
	SDL_ShowWindow(window);
}

void Resources::SetRenderClearColor(){
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
}

//void Resources::Update(const Objects& objects){
void Resources::Update(){
	/*update window size params*/
	SDL_GetWindowSize(window, &windowW, &windowH);

	/*frame rate limiting*/
	unsigned int ticks = SDL_GetTicks();
	unsigned int delta_time = ticks - current_time;
	current_time = ticks;
	int delay_time = 1000/FPS - delta_time;
	if(delay_time>0){
		SDL_Delay(delay_time);
	}

	++frame;

}

