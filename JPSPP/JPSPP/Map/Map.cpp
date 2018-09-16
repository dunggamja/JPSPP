#include "Map.h"

void Map::ProcessUpdateJumpPoint()
{
	//점프포인트 셋팅
	UpdateJumpPoint();

	//직선거리 계산.
	for (size_t idx = Enum2Num(GAME::DIRECTIONS::STRAIGHT_BEGIN); idx < Enum2Num(GAME::DIRECTIONS::STRAIGHT_END); ++idx)
	{
		UpdateDistance_Straight((GAME::DIRECTIONS)idx);
	}

	//대각선거리 계산.
	UpdateDistance_Diagonal(GAME::DIRECTIONS::BOTTOM);
	UpdateDistance_Diagonal(GAME::DIRECTIONS::BOTTOM);
}


void Map::UpdateJumpPoint()
{
	auto CalcTileType = [&](const Tile* pTileInfo)->GAME::TILE_TYPE
	{
		if (nullptr == pTileInfo)
			return GAME::TILE_TYPE::INVALID;

		return (pTileInfo->IsObstacle()) ? GAME::TILE_TYPE::OBSTACLE : GAME::TILE_TYPE::EMPTY;
	};

	for (auto& tileInfo : m_TileRepos)
	{
		tileInfo->UpdateJumpPoint();
	}
}



void Map::InitializeTileRepository()
{
	m_TileRepos.clear();
	m_TileRepos.resize(m_SizeX * m_SizeY);

	for (size_t i = 0; i < m_TileRepos.size(); ++i)
	{
		int x = 0;
		int y = 0;
		std::tie(x, y) = GetXY(i);
		m_TileRepos[i] = std::make_shared<Tile>(weak_from_this(), x, y);
	}
}

void Map::DisplayMap_OnConsole()
{
	auto GetCharacter = [](Tile* pTile)->char 
	{
		if (pTile->IsJumpPoint())
			return 'J';
		else if (pTile->IsObstacle())
			return 'X';
		else
			return 'O';
	};

	for (int y = 0; y < m_SizeY; ++y)
	{
		for (int x = 0; x < m_SizeX; ++x)
		{
			auto pTile = GetTile(x, y);
			if(nullptr != pTile)
			{
				std::cout << GetCharacter(pTile);
			}
		}
		std::cout << std::endl;
	}
}

void Map::SetObstacle(int x, int y)
{
	auto pTile = GetTile(x, y);
	if (pTile)
		pTile->SetIsObstacle(true);
}

void Map::UpdateDistance_Straight(GAME::DIRECTIONS direction)
{
	if (!IsStraightDirection(direction)) return;

	bool isHorizonward = (GAME::DIRECTIONS::LEFT == direction || GAME::DIRECTIONS::RIGHT == direction);

	auto calcDistance = [&](int posX, int posY, GAME::DIRECTIONS distanceDir)->int
	{
		int x = 0, y = 0;
		std::tie(x, y) = GetNeighborPos(posX, posY, distanceDir);

		auto pTile = GetTile(x, y);

		//1.장애물 0
		//2.점프포인트 1
		//3.장애물 또는 점프포인트와의 거리 -1 / +1

		if (IS_NULL(pTile) || pTile->IsObstacle())
		{
			return 0;
		}
		else if (pTile->IsJumpPointAcessFrom(GetReverseDirection(distanceDir)))
		{
			return 1;
		}
		else
		{
			int distValue = pTile->GetDistanceInfo(distanceDir);
			return (0 < distValue) ? distValue + 1 : distValue - 1;
		}


		return 0; 
	};

	if(isHorizonward)
	{
		concurrency::parallel_for(0, m_SizeY, [&](int y)
		{
			bool isRightWard = (GAME::DIRECTIONS::LEFT == direction);

			const int startX	= isRightWard ? 0 : m_SizeX - 1;
			const int endX		= isRightWard ? m_SizeX : -1;
			const int addX		= isRightWard ? 1 : -1;

			for (int x = startX; x != endX; x += addX)
			{
				auto pTile = GetTile(x, y);
				if (IS_NOT_NULL(pTile))
					pTile->SetDistanceInfo(direction, calcDistance(x, y, direction));
			}
		});
	}
	else
	{ 
		concurrency::parallel_for(0, m_SizeX, [&](int x)
		{
			bool isDownWard = (GAME::DIRECTIONS::TOP == direction);

			const int startY = isDownWard ? 0 : m_SizeY - 1;
			const int endY = isDownWard ? m_SizeY : -1;
			const int addY = isDownWard ? 1 : -1;

			for (int y = startY; y != endY; y += addY)
			{
				auto pTile = GetTile(x, y);
				if (IS_NOT_NULL(pTile))
					pTile->SetDistanceInfo(direction, calcDistance(x, y, direction));
			}
		});
	}

}

void Map::UpdateDistance_Diagonal(GAME::DIRECTIONS direction)
{
	if (!IsDiagonalDirection(direction)) return;

	bool isDownWard = (GAME::DIRECTIONS::TOP == direction);

	std::initializer_list<GAME::DIRECTIONS> diagonalDirs;
	if (isDownWard) diagonalDirs = {GAME::DIRECTIONS::LEFTTOP, GAME::DIRECTIONS::RIGHTTOP};
	else            diagonalDirs = {GAME::DIRECTIONS::LEFTBOTTOM, GAME::DIRECTIONS::RIGHTBOTTOM};

	auto calcDistance = [&](int posX, int posY, GAME::DIRECTIONS distanceDir)->int 
	{
		int x = 0, y = 0;
		std::tie(x, y) = GetNeighborPos(posX, posY, distanceDir);
		auto pTile = GetTile(x, y);

		GAME::DIRECTIONS reverseDir = GetReverseDirection(distanceDir);

		//1.장애물 0
		//2.점프포인트 1, 
		//3.점프포인트와 연결된 직선 1
		//4.장애물 또는 점프포인트와의 거리 -1 / +1


		//룩업테이블.
		auto& lookUpDirs = GetLookUpDirs(distanceDir);
		auto& reverseLookUpDirs = GetLookUpDirs(reverseDir);

		//장애물체크. 
		auto checkObstacle = [&]()->bool
		{
			return std::any_of(lookUpDirs.cbegin(), lookUpDirs.cend(), [&](const GAME::DIRECTIONS& dir)->bool
			{
				auto neighborXY = GetNeighborPos(posX, posY, dir);
				auto pTile = GetTile(std::get<0>(neighborXY), std::get<1>(neighborXY));
				return (IS_NULL(pTile) || pTile->IsObstacle());
			});
		};

		//점프포인트 체크
		auto checkJumpPoint = [&](const Tile* pTargetTile)->bool 
		{
			if (IS_NULL(pTargetTile)) 
				return false;

			return std::any_of(reverseLookUpDirs.cbegin(), reverseLookUpDirs.cend(), [&](const GAME::DIRECTIONS dir)->bool 
			{
				return pTargetTile->IsJumpPointAcessFrom(dir);
			});
		};

		//점프포인트와 연결된 직선체크.
		auto checkJumpPointLine = [&](const Tile* pTargetTile)->bool
		{
			if (IS_NULL(pTargetTile)) 
				return false;

			std::any_of(lookUpDirs.cbegin(), lookUpDirs.cend(), [&](const GAME::DIRECTIONS dir)->bool 
			{
				if (IsDiagonalDirection(dir))
					return false;
				
				return (0 < pTargetTile->GetDistanceInfo(dir));
			});

			return false;
		};

		

		//1.장애물 0
		//2.점프포인트 1, 
		//3.점프포인트와 연결된 직선 1
		//4.장애물 또는 점프포인트와의 거리 -1 / +1
		if (checkObstacle())
		{
			return 0;
		}
		else if (checkJumpPoint(pTile) || checkJumpPointLine(pTile))
		{
			return 1;
		}
		else if(IS_NOT_NULL(pTile))
		{
			int distValue = pTile->GetDistanceInfo(distanceDir);
			return (0 < distValue) ? distValue + 1 : distValue - 1;
		}

		return 0;
	};

	int startY = isDownWard ? 0 : m_SizeY - 1;
	int endY   = isDownWard ? m_SizeY : -1;
	int addY   = isDownWard ? 1 : -1;

	for(int y = startY; y != endY; y += addY)
	{ 
		for (int x = 0; x < m_SizeX; ++x)
		{
			for (auto& dir : diagonalDirs)
			{
				auto pTile = GetTile(x, y);
				if (IS_NOT_NULL(pTile))
				{
					pTile->SetDistanceInfo(dir, calcDistance(x, y, dir));
				}
			}
		}
	}

}

std::vector<GAME::POS_INFO> Map::FindPath(GAME::POS_INFO startPos, GAME::POS_INFO endPos)
{
	auto pTileStart = GetTile(std::get<GAME::POS_INFO_X>(startPos), std::get<GAME::POS_INFO_Y>(startPos));
	auto pTileEnd   = GetTile(std::get<GAME::POS_INFO_X>(endPos), std::get<GAME::POS_INFO_Y>(endPos));

	std::vector<GAME::POS_INFO> vecPaths;

	//현재 타일, 부모 타일, 누적된 거리, 진행방향
	using NODE_INFO = std::tuple<Tile*, Tile*, float, GAME::DIRECTIONS>;
	enum NODE_INFO_INDEX {TILE, PARENT, DISTANCE, DIRECTION};

	std::list<NODE_INFO> closelist;
	std::list<NODE_INFO> openlist;
	if (IS_NULL(pTileStart) 
	||  IS_NULL(pTileEnd) 
	||  pTileStart->IsObstacle() 
	||  pTileEnd->IsObstacle()
	||  startPos == endPos)
		return vecPaths;

	openlist.emplace_back(std::make_tuple(
		  pTileStart
		, nullptr
		, 0.f
		, GAME::DIRECTIONS::INVALID));


	//두 지점이 직접 이어지는지 체크합니다. 
	auto checkPos2PosConnected = [&](int startX, int startY, int endX, int endY, GAME::DIRECTIONS dir)->bool
	{
		//1.방향이 맞는지 체크.
		//2.방향이 맞으면 startXY -> endXY 까지 가는데 장애물이 있는지 체크.

		int diffX = endX - startX;
		int diffY = endY - startY;
		int distToGoal = std::max(std::abs(diffX), std::abs(diffY));

		//동일한 지점이면 장애물 없음!
		if (diffX == 0 && diffY == 0)
			return true;


		//방향이 안맞으면 막혀있는 것이다. 
		switch (dir)
		{
		case GAME::DIRECTIONS::TOP:
		{
			if (diffX != 0 || 0 < diffY)
				return false;
		}
		break;
		case GAME::DIRECTIONS::BOTTOM:
		{
			if (diffX != 0 || diffY < 0)
				return false;
		}
		break;
		case GAME::DIRECTIONS::LEFT:
		{
			if (0 < diffX || diffY != 0)
				return false;
		}
		break;
		case GAME::DIRECTIONS::RIGHT:
		{
			if (diffX < 0 || diffY != 0)
				return false;
		}
		break;
		case GAME::DIRECTIONS::LEFTTOP:
		{
			if (std::abs(diffX) != std::abs(diffY)
				|| 0 < diffX
				|| 0 < diffY)
				return false;
		}
		break;
		case GAME::DIRECTIONS::RIGHTTOP:
		{
			if (std::abs(diffX) != std::abs(diffY)
				|| diffX < 0
				|| 0 < diffY)
				return false;
		}
		break;
		case GAME::DIRECTIONS::LEFTBOTTOM:
		{
			if (std::abs(diffX) != std::abs(diffY)
				|| 0 < diffX
				|| diffY < 0)
				return false;
		}
		break;
		case GAME::DIRECTIONS::RIGHTBOTTOM:
		{
			if (std::abs(diffX) != std::abs(diffY)
				|| diffX < 0
				|| diffY < 0)
				return false;
		}
		break;
		}

		//막혀있는 블록을 발견할때까지 재귀적으로 탐색한다. 
		auto pTile = GetTile(startX, startY);
		while (IS_NOT_NULL(pTile))
		{
			int distValue = pTile->GetDistanceInfo(dir);
			if (distValue <= 0)
			{
				//장애물보다 타겟이 더 가까이 있다. 
				if (distToGoal < std::abs(distValue))
					return true;
				else
					return false;
			}
			else
			{
				//다음 점프포인트보다 타겟이 더 가까이 있다. 
				if (distToGoal <= distValue)
				{
					return true;
				}
				else
				{
					//인접한 점프포인트가 있다면 거기서 체크한다. 
					int dirX = 0;
					int dirY = 0;
					std::tie(dirX, dirY) = GetAdjacentDir(dir);
					dirX *= distValue;
					dirY *= distValue;

					pTile = GetTile(pTile->GetPosX() + dirX, pTile->GetPosY() + dirY);
				}
			}
		}

		return false;
	};

	//골 지점과 직선상에 위치한 노드를 찾습니다. 
	auto findNodeForCheckConnect = [&](int startX, int startY, int endX, int endY)->std::list<std::tuple<int,int>> 
	{
		std::list<std::tuple<int, int>> nodeList;
		int diffX = endX - startX;
		int diffY = endY - startY;

		if (diffX == 0 && diffY == 0)
			return nodeList;



		

		return nodeList;
	};

	//0.목표지점이면 종료.
	//1.목표지점과 직선상의 이어지는 지점이 있는지 탐색.  해당지점에서 목표지점으로 이어지면 탐색종료.
	//2.해당지점에서 직선으로 갈수 있는 지점에 목표지점과 직선상으로 이어지는 지점이 있는지 탐색합니다. 해당 노드들 오픈리스트에 추가.
	//3.
	
	while (!openlist.empty())
	{
		auto nodeInfo = openlist.front();
		auto pTile    = std::get<NODE_INFO_INDEX::TILE>(nodeInfo);
		auto nodeDir  = std::get<NODE_INFO_INDEX::DIRECTION>(nodeInfo);
		if (IS_NOT_NULL(pTile))
		{
			auto& lookUpDirs = GetLookUpDirs(nodeDir);
			bool isConnected = std::any_of(lookUpDirs.cbegin(), lookUpDirs.cend(), [&](const GAME::DIRECTIONS& dir)->bool 
			{
				return checkPos2PosConnected(pTile->GetPosX(), pTile->GetPosY(), pTileEnd->GetPosX(), pTileEnd->GetPosY(), dir);
			});

			if (isConnected)
			{

			}
			
		}
		
		openlist.pop_front();
	}

	return vecPaths;
}

