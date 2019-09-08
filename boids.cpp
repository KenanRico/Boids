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
#define TWOPI 6.2831853
#define BOID_CAPACITY 192
#define BOID_EQUILIBRIUM 112

//#define TEST_COLLISION_AVOIDANCE
#define TEST_CROWD_FOLLOWING
#define TEST_CROWD_CENTERING

#include <iostream>
Boids::Boids(SDL_Renderer* renderer): state(OK), boid_count(0), width(0.05f), height(0.05f){
	boid_data.reserve(BOID_CAPACITY);
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
	if(events[EventHandler::S] && boid_data.size()<BOID_CAPACITY){
		AddBoid();
	}
	UpdateFinalVals();
	if(boid_data.size()>BOID_EQUILIBRIUM && resources.Frame()%60==0) TrimOutOfBoundBoids();
	SyncInitialVals();
	UpdateRenderRects(resources);
}

void Boids::Render(SDL_Renderer* renderer) const{
	for(int i=0; i<boid_count; ++i){
		const Boid& data = boid_data[i];
		SDL_RenderCopyEx(renderer, texture, &srcrect, &data.dstrect, data.dirf*(360/TWOPI), nullptr, data.flip);
	}
}

void Boids::AddBoid(){
	++boid_count;
	/*place a boid at a random position in window*/
	float x = (float)(rand()%1000) / 1000.0f;
	float y = (float)(rand()%1000) / 1000.0f;
	double dir = fmod(rand(), TWOPI);
	float speed = (float)(rand()%35+5) / 5000.0f;
	boid_data.push_back((Boid){x,y,dir,x,y,dir,speed,0.02f,PI,0.1f,(SDL_Rect){0,0,0,0}, SDL_FLIP_NONE});
}

typedef struct Point{
	float x;
	float y;
} Point;
/*update [xf, yf, dirf]*/
void Boids::UpdateFinalVals(){
	for(int i=0; i<boid_count; ++i){
		Boid& me = boid_data[i];
		/*find boids in vision*/
		std::vector<Boid const *> boids_in_vision;
		FindBoidsInVision(me, i, &boids_in_vision);
		int boids_in_vision_count = boids_in_vision.size();
		#ifdef TEST_COLLISION_AVOIDANCE
		std::vector<CollisionPoint> collision_points;
		for(int k=0; k<boids_in_vision_count; ++k){
			/*for each boid in vision, compute where the collision will occur assuming me and this boid continues travelling at same velocity*/
		}
		#endif
		#ifdef TEST_CROWD_FOLLOWING
		if(boids_in_vision_count!=0){
			double average_dir = 0.0f;
			for(int k=0; k<boids_in_vision_count; ++k){
				average_dir += boids_in_vision[k]->diri;
			}
			average_dir /= boids_in_vision_count;
			TurnToward(&me, average_dir);
		}
		#endif
		#ifdef TEST_CROWD_CENTERING
		if(boids_in_vision_count!=0){
			Point center{0.0f, 0.0f};
			for(int k=0; k<boids_in_vision_count; ++k){
				center.x += boids_in_vision[k]->xi;
				center.y += boids_in_vision[k]->yi;
			}
			center.x /= boids_in_vision_count;
			center.y /= boids_in_vision_count;
		}
		#endif
		me.dirf = fmod(me.dirf, TWOPI);
		me.xf += me.speed * std::cos(me.dirf);
		if(me.xf<-0.2f){
			me.xf = 1.2f;
		}else if(me.xf>1.2f){
			me.xf = -0.2f;
		}
		me.yf += me.speed * std::sin(me.dirf);
		if(me.yf<-0.2f){
			me.yf = 1.2f;
		}else if(me.yf>1.2f){
			me.yf = -0.2f;
		}
		me.flip = (me.dirf>PI*3/2||me.dirf<PI/2) ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;
	}
}

/*erase boids that are out of bound*/
void Boids::TrimOutOfBoundBoids(){
	boid_data.erase(
		std::remove_if(boid_data.begin(), boid_data.end(), [this](const Boid& boid){return boid.xf<-0.1f||boid.xf>1.1f||boid.yf<-0.1f||boid.yf>1.1f;}),
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
		data.diri = data.dirf;
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

void Boids::FindBoidsInVision(Boid& me, int me_index, std::vector<Boid const *>* boids_in_vision) const{
	for(int j=0; j<boid_count; ++j){
		//same boid
		if(me_index==j){
			continue;
		}
		const Boid& target = boid_data[j];
		//boid is within view distance, and is within FOV
		bool within_view_distance = sqrt((target.xi-me.xi)*(target.xi-me.xi)+(target.yi-me.yi)*(target.yi-me.yi)) < me.vision_dist;
		bool within_FOV = std::abs(atan(target.xi-me.xi/target.yi-me.yi)) < me.FOV/2;
		if(within_view_distance && within_FOV){
			boids_in_vision->push_back(&target);
		}
	}
}

void Boids::TurnToward(Boid* me, double average_dir){
	double turn_dir_cw = average_dir - me->dirf;
	double turn_dir_ccw = (turn_dir_cw>0) ? turn_dir_cw-TWOPI : TWOPI+turn_dir_cw;
	double& turn_angle = (std::abs(turn_dir_cw)<std::abs(turn_dir_ccw)) ? turn_dir_cw : turn_dir_ccw;
	if(std::abs(turn_angle) < me->turning_speed){
		return;
	}
	me->dirf += (turn_angle>0.0) ? me->turning_speed : -me->turning_speed;
}
