#pragma once

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game {
	public:
		GameState state;
		bool keys[1024];
		unsigned int width, height;

		Game(unsigned int width, unsigned int height);
		~Game();

		void Init();

		void ProcessInput(float dt);
		void Update(float dt);
		void Render();
};