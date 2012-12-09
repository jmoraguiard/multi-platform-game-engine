#ifndef _GAME_STATE_H
#define _GAME_STATE_H

class ApplicationManager;

class ApplicationState
{
	public:
		ApplicationState();
		virtual ~ApplicationState();

		virtual bool init() = 0;
		virtual void update(ApplicationManager* game_manager, float elapsed_time) = 0;
		virtual void draw(ApplicationManager* game_manager) = 0;
		virtual void keyboardEvent(ApplicationManager* game_manager, char key_pressed) = 0;

		virtual void onResize(ApplicationManager* game_manager, int width, int height) = 0;

		void changeState(ApplicationManager* game_manager, ApplicationState* game_state);
};

#endif
