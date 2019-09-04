#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdint.h>

#include <SDL2/SDL.h>


class Resources{
	private:
		SDL_Renderer* renderer;
		SDL_Window* window;
		int windowW; int windowH;
		unsigned int current_time;
		int FPS;
		uint64_t frame;
		const float world_width; const float world_height;
		uint8_t state;
	public:
		Resources();
		~Resources();
	private:
		Resources(const Resources&) = delete;
		Resources& operator=(const Resources&) = delete;

	public:
		void CreateWindow();
		void SetRenderClearColor();
		inline uint8_t State() const { return state; }
		void Update();
		inline void QueryWindowDimension(int* w, int* h) const { *w=windowW; *h=windowH; }
		inline void QueryWorldSize(float* w, float* h) const { *w=world_width; *h=world_height; }
		inline SDL_Renderer* Renderer() const { return renderer; }
		inline uint64_t Frame() const { return frame; }
};

#endif
