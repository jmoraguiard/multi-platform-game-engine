#ifndef _SPLASH_SCREEN_STATE_H_
#define _SPLASH_SCREEN_STATE_H_

#include "applicationState.h"

class SplashScreenState: public ApplicationState
{
	public:
		SplashScreenState();
		virtual ~SplashScreenState();
		static SplashScreenState* Instance();

		virtual bool init();
		virtual void update(ApplicationManager* game_manager, float elapsed_time);
		virtual void draw(ApplicationManager* game_manager);
		virtual void keyboardEvent(ApplicationManager* game_manager, char key_pressed);

		virtual void onResize(ApplicationManager* game_manager, int width, int height);

	private:
		static SplashScreenState splash_screen_state_; //singleton

		float angle_rotation_;
};

#endif
