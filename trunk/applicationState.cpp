#ifdef _WIN32
#include <windows.h>
#endif

#include "applicationState.h"

#include "applicationManager.h"

ApplicationState::ApplicationState() {

}

ApplicationState::~ApplicationState() {

}

void ApplicationState::changeState(ApplicationManager* game_manager, ApplicationState* game_state) {
	game_manager->changeState(game_state);
}


