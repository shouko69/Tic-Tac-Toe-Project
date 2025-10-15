#include "View.h"
#include "Control.h"
#include <ctype.h>
#include <conio.h>
#include "Model.h"
#include <stdio.h>

//  git lz moe mi
int main() {
	FixConsoleWindow();
	StartGame();
	bool validEnter = true;
	while (1) {

		_COMMAND = toupper(_getch());
		if (_COMMAND == 27) { // 27 la ma ESC
			ExitGame();
			return 0;
		}
		else {
			if (_COMMAND == 'A') MoveLeft();
			else if (_COMMAND == 'S') MoveDown();
			else if (_COMMAND == 'D') MoveRight();
			else if (_COMMAND == 'W') MoveUp();
			else if (_COMMAND == 13) { // 13 la ma Enter
				switch (CheckBoard(_X, _Y)) {
				case -1:
					printf("X");
					break;
				case 1:
					printf("O");
					break;
				case 0:
					validEnter = false;
					break; // Them break de khong chay vao code tiep theo
				}

				if (validEnter) {
					int status = ProcessFinish(TestBoard());
					if (status == -1 || status == 1 || status == 0) {
						if (AskContinue() != 'Y') {
							ExitGame();
							return 0;
						}
						else {
							StartGame();
						}
					}
				}
				validEnter = true; // reset lai validEnter cho lan lap tiep theo
			}
		}
	}
	return 0;
}