#include "resources.h"
#include "boids.h"

#include <iostream>

#include <SDL2/SDL.h>

#ifdef main
#undef main
#endif


void Update(Resources*, EventHandler*, Boids*);
void Render(Boids const *, SDL_Renderer*);

int main(){
	Resources resources;
	EventHandler events;
	Boids boids(resources.Renderer());
	resources.SetRenderClearColor();
	resources.CreateWindow();
	while((resources.State() | boids.State() | events.State()) == 0){
		Update(&resources, &events, &boids);
		Render(&boids, resources.Renderer());
	}
	std::cout<<(int)resources.State()<<" "<<(int)boids.State()<<"\n";
    return 0;
}

void Update(Resources* resources, EventHandler* events, Boids* boids){
	resources->Update();
	events->Update();
	boids->Update(*resources, *events);
}

void Render(Boids const * boids, SDL_Renderer* renderer){
	SDL_RenderClear(renderer);
	boids->Render(renderer);
	SDL_RenderPresent(renderer);
}
