#include "Global.h"
#include "Map/Map.h"

int main()
{
	auto pMap = std::make_shared<Map>(16, 16);
	pMap->InitializeTileRepository();

	{
		auto timeS = std::chrono::system_clock::now();

		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<> distX(1, pMap->GetSizeX() - 2);
		std::uniform_int_distribution<> distY(1, pMap->GetSizeY() - 2);

		//for (int i = 0; i < 80; ++i)
		//{
		//	pMap->SetObstacle(distX(gen), distY(gen));
		//}

		//16,16 사이즈의 맵이다.

		pMap->SetObstacle(1, 0);
		pMap->SetObstacle(1, 1);
		pMap->SetObstacle(1, 2);
		pMap->SetObstacle(1, 3);

		for (int y = 1; y < 16; ++y)
		{
			for (int x = 3; x < 15; x += 4)
				pMap->SetObstacle(x, y);
		}

		for (int y = 0; y < 15; ++y)
		{
			for(int x = 1; x < 15; x += 4)
				pMap->SetObstacle(x, y);
		}
		


		pMap->ProcessUpdateJumpPoint();
		pMap->DisplayMap_OnConsole();


		auto timeE = std::chrono::system_clock::now();

		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(timeE - timeS).count() << std::endl;
	}

	{
		auto timeS = std::chrono::system_clock::now();
		auto vecPath = pMap->FindPath(std::make_tuple(0, 0), std::make_tuple(15, 15));
		auto timeE = std::chrono::system_clock::now();
		for (auto& path : vecPath)
		{
			std::cout << "[" << std::get<0>(path) << ", " << std::get<1>(path) << "] ";
		}
		std::cout << std::endl;
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(timeE - timeS).count() << std::endl;
	}

	{
		int input = 0;
		std::cin >> input;
	}

	return 0;
}