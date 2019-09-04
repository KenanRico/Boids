#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <stdint.h>
#include <array>

#include <SDL2/SDL.h>

class EventHandler{
	public:
		enum Keys { S=0, W=1, D=2, A=3 };
		static constexpr int COUNT = 4;
	private:
		std::array<bool, COUNT> keys;
		SDL_Event events;
		uint8_t state;

	public:
		EventHandler();
		~EventHandler();
	private:
		EventHandler(const EventHandler&) = delete;
		EventHandler& operator=(const EventHandler&) = delete;
	public:
		void Update();
		inline bool operator[](uint8_t i) const { return keys[i]; }
		inline uint8_t State() const { return state; }

};

#endif
