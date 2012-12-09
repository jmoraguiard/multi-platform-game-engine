#ifdef _WIN32
#include <windows.h>
#endif

#include "clock.h"

#include <ctime>

Clock::Clock():
	total_time_(0.0f),
	last_time_(0.0f),
	is_paused_(false) {
}

Clock::~Clock() {
}

void Clock::init() {
	
	last_time_ = GetTickCount() / 1000.0f;

}

float Clock::calculateDeltaSeconds() {

	float currentTime = float(GetTickCount()) / 1000.0f;
    float seconds = float(currentTime - last_time_);
    last_time_ = currentTime;

    return seconds;

}

void Clock::update(float dt_real_seconds) {

	if(!is_paused_)
		total_time_ += dt_real_seconds;

}

void Clock::setPaused(bool is_paused) {

	is_paused_ = is_paused;

}

bool Clock::isPaused() {

	return is_paused_;

}