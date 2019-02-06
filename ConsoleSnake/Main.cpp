#include <iostream>
#include <conio.h>
#include <string>
#include <random>
#include <ctime>
#include <map>
#include <utility>
#include <windows.h>


// functions
void Init();
void SetConsolePos(int x, int y);
void Draw();
void Input();
void GameOver();
void Logic();
void Move();
void CreateEat();
void CreateSnake();
void SnakeResize();


//const
const int FIELD_WIDTH = 30;
const int FIELD_HEIGHT = 20;

//var
bool snake_increased;
bool need_draw;
bool pause_pressed;
int eat_count;
int snake_start = 0;
int eat_x = 5;
int eat_y = 5;
int direction = 2;
int direction_buf;
int timer = 0;
int game_state;
char field[FIELD_HEIGHT][FIELD_WIDTH];
std::map<int, std::pair<int, int>> snake;
std::clock_t start;
double duration;
double game_time_loop = 0.4; // ms, > 0.2
double time_loop;

enum GameStates {
	Play = 0,
	Pause = 1,
	Menu = 2,
	Game_Over = 3,
	Exit = 4,
	New = 5
} ;

int main() {
	Init();
	game_state = GameStates::Menu;

	while (game_state != GameStates::Exit) {
		if (need_draw) { Draw(); }	
		Input();
		Logic();
		Sleep(2);
	}
	return 0;
}


void CreateEat() {
	bool eat_created = false;
	while (!eat_created) {
		eat_x = std::rand() % (FIELD_WIDTH - 2) + 1;
		eat_y = std::rand() % (FIELD_HEIGHT - 2) + 1;

		eat_created = true;
		for (int i = 0; i < snake.size(); i++) {
			if (snake[i].first == eat_y && snake[i].second == eat_x) {
				eat_created = false;
			}
		}
	}

	field[eat_y][eat_x] = '*';
}


void CreateSnake() {
	int x, y;

	x = std::rand() % (FIELD_WIDTH - 2) + 1;
	y = std::rand() % (FIELD_HEIGHT - 2) + 1;

	snake[0] = { y, x };
	field[snake[0].first][snake[0].second] = 'O';
	snake_start = 0;
}


void Init() {
	system("cls");
	snake_increased = false;
	need_draw = true;
	eat_count = 0;
	start = std::clock();
	game_state = GameStates::Play;
	snake.clear();
	snake_start = 0;

	srand(time(NULL));

	for (int i = 0; i < FIELD_HEIGHT; i++) {
		for (int j = 0; j < FIELD_WIDTH; j++) {
			if (i == 0 || (i == FIELD_HEIGHT - 1)) {
				field[i][j] = '#';
			}
			else if (j == 0 || (j == FIELD_WIDTH - 1)) {
				field[i][j] = '#';
			}
			else {
				field[i][j] = ' ';
			}
		}
	}

	CreateEat();
	CreateSnake();
}

void SetConsolePos(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


void Draw() {
	//system("cls");
	if (game_state == GameStates::Play) {
		SetConsolePos(0, 0);
		for (int i = 0; i < FIELD_HEIGHT; i++) {
			for (int j = 0; j < FIELD_WIDTH; j++) {
				std::cout << field[i][j];
			}
			std::cout << std::endl;
		}

		std::cout << "Score: " << eat_count << std::endl;
		std::cout << "Time: " << time_loop << std::endl;
	} else if (game_state == GameStates::Pause) {
		//system("cls");
		SetConsolePos(12, 7);
		std::cout << "PAUSE" << std::endl << std::endl;
	}
	else if (game_state == GameStates::Menu) {
		system("cls");
		std::cout << "###### MENU ######" << std::endl;
		std::cout << "N - New Game" << std::endl;
		std::cout << "M - Menu" << std::endl;
		std::cout << "P - Pause Game" << std::endl;
		std::cout << "Q - Exit" << std::endl;
		std::cout << std::endl;
		std::cout << "#### CONTROL #####" << std::endl;
		std::cout << "W - top" << std::endl;
		std::cout << "S - bottom" << std::endl;
		std::cout << "A - left" << std::endl;
		std::cout << "D - right" << std::endl;
	}

	if (game_state == GameStates::Game_Over) {
		SetConsolePos(0, 0);
		for (int i = 0; i < FIELD_HEIGHT; i++) {
			for (int j = 0; j < FIELD_WIDTH; j++) {
				std::cout << field[i][j];
			}
			std::cout << std::endl;
		}

		std::cout << "Score: " << eat_count << std::endl;
		std::cout << "Time: " << time_loop << std::endl;
		std::cout << "GAME OVER" << std::endl;
		std::cout << "Press 'N' for New Game" << std::endl;
	}

	need_draw = false;
}

// Windows Only
void Input() {
	if ((GetKeyState('W') & 0x8000 || GetKeyState(VK_UP) & 0x8000) && direction != 1) {
		direction_buf = 0;
	}
	else if ((GetKeyState('S') & 0x8000 || GetKeyState(VK_DOWN) & 0x8000) && direction != 0) {
		direction_buf = 1;
	}
	else if ((GetKeyState('A') & 0x8000 || GetKeyState(VK_LEFT) & 0x8000) && direction != 3) {
		direction_buf = 2;
	}
	else if ((GetKeyState('D') & 0x8000 || GetKeyState(VK_RIGHT) & 0x8000) && direction != 2) {
		direction_buf = 3;
	}
	else if (GetKeyState('Q') & 0x8000) {
		game_state = GameStates::Exit;
	}
	else if (GetKeyState('N') & 0x8000) {
		game_state = GameStates::New;
	}
	else if (GetKeyState('M') & 0x8000) {
		game_state = GameStates::Menu;
		need_draw = true;
	}

	if (GetKeyState('P') & 0x8000) {
		pause_pressed = true;
	}
	else if (pause_pressed) {
		if (game_state == GameStates::Play) {
			game_state = GameStates::Pause;
			need_draw = true;
		}
		else if (game_state == GameStates::Pause) {
			game_state = GameStates::Play;
			need_draw = true;
		}
		pause_pressed = false;
	}
}



void Move() {
	int last = (snake_start != 0) ? snake_start - 1 : snake.size() - 1;

	field[snake[last].first][snake[last].second] = ' ';
	if (snake.size() > 1) {
		field[snake[snake_start].first][snake[snake_start].second] = 'o';
	}

	if (direction == 0) {
		snake[last].first = snake[snake_start].first - 1;
		snake[last].second = snake[snake_start].second;
	}
	else if (direction == 1) {
		snake[last].first = snake[snake_start].first + 1;
		snake[last].second = snake[snake_start].second;
	}
	else if (direction == 2) {
		snake[last].first = snake[snake_start].first;
		snake[last].second = snake[snake_start].second - 1;
	}
	else if (direction == 3) {
		snake[last].first = snake[snake_start].first;
		snake[last].second = snake[snake_start].second + 1;
	}

	snake_start = last;
	field[snake[snake_start].first][snake[snake_start].second] = 'O';
}


void SnakeResize() {
	if (snake_start != 0) {
		int snake_it = snake_start;
		std::map<int, std::pair<int, int>> temp;

		for (int i = 0; i < snake.size(); i++) {
			temp[i] = snake[snake_it];
			snake_it++;
			if (snake_it == snake.size()) {
				snake_it = 0;
			}
		}

		snake = temp;
	}

	snake_start = 0;
	snake[snake.size()] = snake[0];
	snake_increased = true;
}


void Logic(){
	switch (game_state)
	{
	case GameStates::Play:
		if (snake[snake_start].second == 0 || snake[snake_start].second == (FIELD_WIDTH - 1) || snake[snake_start].first == 0 || snake[snake_start].first == (FIELD_HEIGHT - 1)) {
			GameOver();
		}

		for (int i = 0; i < snake.size(); i++) {
			if (!snake_increased && i != snake_start && snake[snake_start].second == snake[i].second && snake[snake_start].first == snake[i].first) {
				GameOver();
			}
		}

		if (snake[snake_start].second == eat_x && snake[snake_start].first == eat_y) {
			eat_count++;
			CreateEat();
			SnakeResize();
		}

		if (duration > game_time_loop) {
			start = std::clock();
			direction = direction_buf;
			Move();
			snake_increased = false;
			need_draw = true;
			time_loop = duration;
		}
		break;
	case GameStates::New:
		Init();
		break;
	default:
		break;
	}

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
}


void GameOver() {
	game_state = GameStates::Game_Over;
	need_draw = true;
}