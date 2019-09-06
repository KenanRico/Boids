#ifndef BOIDS_H
#define BOIDS_H

#include "resources.h"
#include "eventhandler.h"

#include <stdint.h>
#include <vector>

#include <SDL2/SDL.h>

class Boids{
	private:
		typedef struct _Boid{
			float xi; float yi; double diri;
			float xf; float yf; double dirf;
			float speed; float turning_speed;
			float FOV; float vision_dist;
			SDL_Rect dstrect;
		} Boid;
	private:
		/*shared properties*/
		SDL_Texture* texture;
		SDL_Rect srcrect; 
		float width;
		float height;
		/*individual properties*/
		std::vector<Boid> boid_data;
		/*boid class properties*/
		int boid_count;
		uint8_t state;

	public:
		Boids(SDL_Renderer*);
		~Boids();
	private:
		Boids();
		Boids(const Boids&) = delete;
		Boids& operator=(const Boids&) = delete;

	public:
		void Update(const Resources&, const EventHandler&);
		void Render(SDL_Renderer*) const;
		inline uint8_t State() const { return state; }
	private:
		void AddBoid();
		void UpdateFinalVals();
		void SyncInitialVals();
		void UpdateRenderRects(const Resources&);
		void TrimOutOfBoundBoids();
		void FindBoidsInVision(Boid&, int, std::vector<Boid const *>*) const;
		void TurnToward(Boid*, double);
};

#endif
