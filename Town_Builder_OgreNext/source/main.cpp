#include <iostream>
#include "Game.h"

bool g_running = false;

int main()
{
	std::unique_ptr<CGame> game = std::make_unique<CGame>();

	try
	{

		game->Setup();
		g_running = true;

	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred during execution: " << e.what() << '\n';
		system("PAUSE");

		return 1;
	}

	while (g_running)
	{
		try
		{
			game->Update();
			game->Render();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error occurred during execution: " << e.what() << '\n';
			system("PAUSE");

			return 1;
		}

	}

	system("PAUSE");

	game.release();
}
