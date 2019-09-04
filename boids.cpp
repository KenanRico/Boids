#include "boids.h"
#include "resources.h"
#include "eventhandler.h"

#include <cmath>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/extensions/SDL_image.h>


#define OK 0
#define IMG_LOAD_FAIL 1
#define TEX_LOAD_FAIL 2

#define PI 3.1415926


Boids::Boids(SDL_Renderer* renderer): state(OK), boid_count(0), width(0.05f), height(0.05f){
	SDL_Surface* surf = IMG_Load("Texture/boidtexture.png");
	if(surf==nullptr){
		state = IMG_LOAD_FAIL;
		return;
	}
	texture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if(texture==nullptr){
		state = TEX_LOAD_FAIL;
		return;
	}
	srcrect.x = 0;
	srcrect.y = 0;
	SDL_QueryTexture(texture, nullptr, nullptr, &srcrect.w, &srcrect.h);
}

Boids::~Boids(){
	SDL_DestroyTexture(texture);
}

void Boids::Update(const Resources& resources, const EventHandler& events){
	/*event to add a boid*/
	if(events[EventHandler::S]){
		AddBoid();
	}
	/*update [xf, yf, direction]*/
	for(int i=0; i<boid_count; ++i){
		Data& data = boid_data[i];
		data.xf += data.speed * std::cos(data.direction);
		data.yf += data.speed * std::sin(data.direction);
		data.direction = data.direction;
	}
	/*sync [xi, yi] with [xf, yf]*/
	for(int i=0; i<boid_count; ++i){
		Data& data = boid_data[i];
		data.xi = data.xf;
		data.yi = data.yf;
	}
	/*update rectangles*/
	int window_w = 0;
	int window_h = 0;
	resources.QueryWindowDimension(&window_w, &window_h);
	for(int i=0; i<boid_count; ++i){
		Data& data = boid_data[i];
		data.dstrect.x = window_w * data.xi;
		data.dstrect.y = window_h * data.yi;
		data.dstrect.w = window_w * width;
		data.dstrect.h = window_h * height;
	}

}

#include <iostream>
void Boids::Render(SDL_Renderer* renderer){
	for(int i=0; i<boid_count; ++i){
		const Data& data = boid_data[i];
		SDL_RenderCopyEx(renderer, texture, &srcrect, &data.dstrect, data.direction, nullptr, SDL_FLIP_NONE);
	}
}

void Boids::AddBoid(){
	++boid_count;
	/*place a boid at a random position in window*/
	float x = (float)(rand()%1000) / 1000.0f;
	float y = (float)(rand()%1000) / 1000.0f;
	double dir = fmod(rand(), 2*PI);
	boid_data.push_back((Data){x,y,x,y,dir,0.001f,PI,0.002f,(SDL_Rect){0,0,0,0}});
}
