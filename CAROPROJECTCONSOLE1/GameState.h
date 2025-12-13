#pragma once

#include "model.h"


enum GameState {
	MENU,
	PLAY_1P, // comming soon
	PLAY_2P, // new
	NEW_GAME_MODE, // new 
	PLAYER_NAME_INPUT_1P, // new
	PLAYER_NAME_INPUT_2P, // new
	GAME_OVER, // new
	LOAD,
	SETTINGS,
	GUIDE,
	ABOUT,
	EXIT,
	PAUSE,
	SAVE,
};



extern GameState currentState;



