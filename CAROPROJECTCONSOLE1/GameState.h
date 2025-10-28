#pragma once

enum GameState {
	MENU,
	PLAY,
	LOAD,
	SETTINGS,
	GUIDE,
	ABOUT,
	EXIT,
	PAUSE,
};

extern GameState currentState;
