#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "applicationManager.h"

#include "applicationState.h"
#include "splashScreenState.h"

ApplicationManager::ApplicationManager() {
	game_state_ = NULL;
	changeState( SplashScreenState::Instance() );
}

ApplicationManager::~ApplicationManager() {

}

bool ApplicationManager::init() {
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	return game_state_->init();

}

void ApplicationManager::update(float elapsed_time) {
	game_state_->update(this, elapsed_time);
}

void ApplicationManager::draw() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	game_state_->draw(this);

}

void ApplicationManager::processKeyboardEvent(char key_pressed) {
	game_state_->keyboardEvent(this, key_pressed);
}

void ApplicationManager::onResize(int width, int height) {

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, float(width) / float(height), 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	game_state_->onResize(this, width, height);

}

void ApplicationManager::changeState(ApplicationState* game_state) {
	if (game_state != game_state_) {
		game_state_ = game_state;
		game_state_->init();
	}
}



