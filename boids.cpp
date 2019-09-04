#include "boids.h"
#include "resources.h"
#include "eventhandler.h"

#include <cmath>
#include <math.h>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/extensions/SDL_image.h>


#define OK 0
#define IMG_LOAD_FAIL 1
#define TEX_LOAD_FAIL 2

#define PI 3.1415926

//#define TEST_COLLISION_AVOIDANCE


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
	UpdateFinalVals();
	if(resources.Frame()%180==0) TrimOutOfBoundBoids();
	SyncInitialVals();
	UpdateRenderRects(resources);
}

void Boids::Render(SDL_Renderer* renderer) const{
	for(int i=0; i<boid_count; ++i){
		const Boid& data = boid_data[i];
		SDL_RenderCopyEx(renderer, texture, &srcrect, &data.dstrect, data.direction*(360/2/PI), nullptr, SDL_FLIP_NONE);
	}
}

void Boids::AddBoid(){
	++boid_count;
	/*place a boid at a random position in window*/
	float x = (float)(rand()%1000) / 1000.0f;
	float y = (float)(rand()%1000) / 1000.0f;
	double dir = fmod(rand(), 2*PI);
	boid_data.push_back((Boid){x,y,x,y,dir,0.001f,PI,0.002f,(SDL_Rect){0,0,0,0}});
}

/*update [xf, yf, direction]*/
void Boids::UpdateFinalVals(){
	for(int i=0; i<boid_count; ++i){
		Boid& me = boid_data[i];
		/*steet clear of boids in vision*/
		#ifdef TEST_COLLISION_AVOIDANCE
		std::vector<Boid*> boids_in_vision;
		for(int j=0; j<boid_count; ++j){
			//same boid
			if(i==j){
				continue;
			}
			Boid& target = boid_data[j];
			//boid is within view distance, and is within FOV
			bool within_view_distance = sqrt((target.x-me.x)*(target.x-me.x)+(target.y-me.y)*(target.y-me.y)) < me.vision_distance;
			bool within_FOV = std::abs(atan(target.x-me.x/target.y-me.y)-me.direction) < data.FOV/2;
			if(within_view_distance && within_FOV){
				boids_in_vision.push_back(&target);
			}
		}
		for(int k=0; k<boids_in_vision.size(); ++k){
			/*for each boid in vision, compute where the collision will occur assuming me and this boid continues travelling at same velocity*/
		}
		#endif
		me.xf += me.speed * std::cos(me.direction);
		me.yf += me.speed * std::sin(me.direction);
		me.direction = me.direction;
	}
}

/*erase boids that are out of bound*/
void Boids::TrimOutOfBoundBoids(){
	boid_data.erase(
		std::remove_if(boid_data.begin(), boid_data.end(), [this](const Boid& boid){return boid.xf<-0.2f||boid.xf>1.2f||boid.yf<-0.2f||boid.yf>1.2f;}),
		boid_data.end()
	);
	boid_count = boid_data.size();
}

/*sync [xi, yi] with [xf, yf]*/
void Boids::SyncInitialVals(){
	for(int i=0; i<boid_count; ++i){
		Boid& data = boid_data[i];
		data.xi = data.xf;
		data.yi = data.yf;
	}
}

void Boids::UpdateRenderRects(const Resources& resources){
	/*update rectangles*/
	int window_w = 0;
	int window_h = 0;
	resources.QueryWindowDimension(&window_w, &window_h);
	for(int i=0; i<boid_count; ++i){
		Boid& data = boid_data[i];
		data.dstrect.x = window_w * (data.xi+width/2);
		data.dstrect.y = window_h * (data.yi+height/2);
		data.dstrect.w = window_w * width;
		data.dstrect.h = window_h * height;
	}

}
