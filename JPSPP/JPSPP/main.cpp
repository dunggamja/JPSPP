#include "Global.h"
#include "Map/Map.h"

int main()
{
	auto pMap = std::make_shared<Map>(16, 16);
	pMap->InitializeTileRepository();
	
	auto timeS = std::chrono::system_clock::now();

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distX(0, pMap->GetSizeX() - 1);
	std::uniform_int_distribution<> distY(0, pMap->GetSizeY() - 1);

	/*for (int i = 0; i < 8; ++i)
	{
		pMap->SetObstacle(distX(gen), distY(gen));
	}*/

	pMap->SetObstacle(1, 1);
	pMap->SetObstacle(1, 2);
	pMap->SetObstacle(1, 3);


	pMap->ProcessUpdateJumpPoint();
	pMap->DisplayMap_OnConsole();


	auto timeE = std::chrono::system_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(timeE - timeS).count() << std::endl;

	{
		int input = 0;
		std::cin >> input;
	}

	return 0;
}