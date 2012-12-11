#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "splashScreenState.h"

SplashScreenState SplashScreenState::splash_screen_state_;

SplashScreenState::SplashScreenState() {
	angle_rotation_ = 0;
}

SplashScreenState::~SplashScreenState() {

}

SplashScreenState* SplashScreenState::Instance() {
	return (&splash_screen_state_);
}

bool SplashScreenState::init() {

	angle_rotation_ = 0;

	return true;

}

void SplashScreenState::update(ApplicationManager* game_manager, float elapsed_time) {

	const float SPEED = 15.0f;

	angle_rotation_ += SPEED * elapsed_time;
	if (angle_rotation_ > 360.0f)
		angle_rotation_ -= 360.0f;

}

void SplashScreenState::draw(ApplicationManager* game_manager) {

	glRotatef(angle_rotation_, 0, 0, 1);

	glBegin(GL_TRIANGLES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, -0.5f, -4.0f);
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, -0.5f, -4.0f);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.5f, -4.0f);
	glEnd();

}

void SplashScreenState::keyboardEvent(ApplicationManager* game_manager, char key_pressed) {

	switch(key_pressed)
	{
		/*case VK_F1:
			changeState(game_manager, SplashScreenState::Instance());
			break;*/
	}

}

void SplashScreenState::onResize(ApplicationManager* game_manager, int width, int height) {

}
