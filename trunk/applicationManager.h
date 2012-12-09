#ifndef _GAME_MANAGER_H
#define _GAME_MANAGER_H

class ApplicationState;

class ApplicationManager
{
	public:
		ApplicationManager();
		virtual ~ApplicationManager();

		bool init();
		void update(float elapsed_time);
		void draw();
		void processKeyboardEvent(char key_pressed);

		void onResize(int width, int height);

		void changeState(ApplicationState* game_state);

	private:
		ApplicationState* game_state_;
};

#endif
