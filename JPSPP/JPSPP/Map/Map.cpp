#include "Map.h"
#include "Tile/Tile.h"


void Map::ProcessUpdateJumpPoint()
{
	UpdateAllJumpPoint();

	UpdateStraightDistance(GAME::DIRECTIONS::LEFT);
	UpdateStraightDistance(GAME::DIRECTIONS::RIGHT);
	UpdateStraightDistance(GAME::DIRECTIONS::TOP);
	UpdateStraightDistance(GAME::DIRECTIONS::BOTTOM);

	UpdateDiagonalDistance(GAME::DIRECTIONS::BOTTOM);
	UpdateDiagonalDistance(GAME::DIRECTIONS::TOP);
}


void Map::UpdateAllJumpPoint()
{
	auto CalcTileType = [&](const Tile* pTileInfo)->GAME::TILE_TYPE
	{
		if (nullptr == pTileInfo)
			return GAME::TILE_TYPE::INVALID;

		return (pTileInfo->IsObstacle()) ? GAME::TILE_TYPE::OBSTACLE : GAME::TILE_TYPE::EMPTY;
	};

	for (auto& tileInfo : m_TileRepos)
	{
		std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)> neighborTileType;
		for (size_t dirIdx = 0; dirIdx < Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT); ++dirIdx)
		{
			int neighborX = tileInfo->GetPosX() + std::get<0>(GAME::DIR_REFERENCE_POS[dirIdx]);
			int neighborY = tileInfo->GetPosY() + std::get<1>(GAME::DIR_REFERENCE_POS[dirIdx]);

			neighborTileType[dirIdx] = CalcTileType(GetTile(neighborX, neighborY));
		}

		tileInfo->UpdateJumpPointAllowEntry(neighborTileType);
	}
}

void Map::UpdateStraightDistance(GAME::DIRECTIONS direction)
{
	if (direction < GAME::DIRECTIONS::STRAIGHT_BEGIN || GAME::DIRECTIONS::STRAIGHT_END <= direction)
		return;

	auto UpdateDistanceInfo = [](Tile& tileInfo, const GAME::DIRECTIONS updateDir, const GAME::DIRECTIONS entryDir, const int distance, const bool isConnectJP)->std::tuple<int, bool>
	{
		tileInfo.SetDistanceInfo(updateDir, distance);

		if (tileInfo.IsObstacle())
			return std::make_tuple(0, false);
		else if (tileInfo.GetJumpPointAllowEntry(entryDir))
			return std::make_tuple(0, true);
		else
			return std::make_tuple((isConnectJP) ? distance + 1 : distance - 1, isConnectJP);
	};

	bool isHorizonward = (GAME::DIRECTIONS::LEFT == direction || GAME::DIRECTIONS::RIGHT == direction);

	if (isHorizonward)
	{
		concurrency::parallel_for(0, m_SizeY, [&](int y)
		{
			int startX = (GAME::DIRECTIONS::LEFT == direction) ? 0 : m_SizeX - 1;
			int endX = (GAME::DIRECTIONS::LEFT == direction) ? m_SizeX : -1;

			int addValue = (GAME::DIRECTIONS::LEFT == direction) ? 1 : -1;

			GAME::DIRECTIONS entryDir = (GAME::DIRECTIONS::LEFT == direction) ? GAME::DIRECTIONS::RIGHT : GAME::DIRECTIONS::LEFT;

			int  distance = 0;
			bool isConnectJP = false;

			for (int x = startX; x != endX; x += addValue)
			{
				auto pTile = GetTile(x, y);
				if (nullptr != pTile)
					std::tie(distance, isConnectJP) = UpdateDistanceInfo((*pTile), direction, entryDir, distance, isConnectJP);
			}
		});
	}
	else
	{
		concurrency::parallel_for(0, m_SizeX, [&](int x)
		{
			int startY = (GAME::DIRECTIONS::TOP == direction) ? 0 : m_SizeY - 1;
			int endY = (GAME::DIRECTIONS::TOP == direction) ? m_SizeY : -1;

			int addValue = (GAME::DIRECTIONS::TOP == direction) ? 1 : -1;

			GAME::DIRECTIONS entryDir = (GAME::DIRECTIONS::TOP == direction) ? GAME::DIRECTIONS::BOTTOM : GAME::DIRECTIONS::TOP;

			int  distance = 0;
			bool isConnectJP = false;

			for (int y = startY; y != endY; y += addValue)
			{
				auto pTile = GetTile(x, y);
				if (nullptr != pTile)
					std::tie(distance, isConnectJP) = UpdateDistanceInfo((*pTile), direction, entryDir, distance, isConnectJP);
			}
		});
	}
}

void Map::UpdateDiagonalDistance(GAME::DIRECTIONS direction)
{
	if (direction != GAME::DIRECTIONS::TOP && direction != GAME::DIRECTIONS::BOTTOM)
		return;

	int startY = (GAME::DIRECTIONS::TOP == direction) ? 0 : m_SizeY - 1;
	int endY = (GAME::DIRECTIONS::TOP == direction) ? m_SizeY : -1;
	int addValue = (GAME::DIRECTIONS::TOP == direction) ? 1 : -1;


	GAME::DIRECTIONS diagonalDirections[2];
	if (GAME::DIRECTIONS::TOP == direction)
	{
		diagonalDirections[0] = GAME::DIRECTIONS::LEFTTOP;
		diagonalDirections[1] = GAME::DIRECTIONS::RIGHTTOP;
	}
	else
	{
		diagonalDirections[0] = GAME::DIRECTIONS::LEFTBOTTOM;
		diagonalDirections[1] = GAME::DIRECTIONS::RIGHTBOTTOM;
	}

	auto CalcDiagonalDistance = [&](Tile& tileInfo, GAME::DIRECTIONS diagDir)->int
	{
		auto& lookUpDirections = GAME::DIR_LOOKUP_TABLES[Enum2Num(diagDir)];

		if (std::any_of(lookUpDirections.begin(), lookUpDirections.end(),
			[&](const std::remove_reference_t<decltype(lookUpDirections)>::value_type& lookUpValue)->bool
		{
			return IsStraightDirection(lookUpValue) && (0 == tileInfo.GetDistanceInfo(lookUpValue));
		}))
		{
			//관련 직선방향이 하나라도 막혀있다면 대각선으로 갈수없는곳이다.  
			return 0;
		}

		auto& refPosInfo = GAME::DIR_REFERENCE_POS[Enum2Num(diagDir)];
		auto  pNeighborTile = GetTile(tileInfo.GetPosX() + std::get<0>(refPosInfo), tileInfo.GetPosY() + std::get<1>(refPosInfo));

		if (nullptr == pNeighborTile)
			return 0;

		if (pNeighborTile->IsObstacle())
			return 0;
		else if (pNeighborTile->CheckJumpPointAllowEntry(diagDir))
			return 1;
		else
		{
			return std::accumulate(lookUpDirections.begin(), lookUpDirections.end()
				, 0
				, [&](int distance, const std::remove_reference_t<decltype(lookUpDirections)>::value_type& lookUpValue)->int
			{
				int tempDistance = pNeighborTile->GetDistanceInfo(lookUpValue);
				if (IsDiagonalDirection(lookUpValue))
				{
					distance = (tempDistance <= 0) ? tempDistance - 1 : tempDistance + 1;
				}
				else if (0 < tempDistance) //점프노드와 직선으로 연결된 타일이다. 
				{
					distance = 1;
				}
				return distance;
			});
		}

		return 0;
	};




	for (int y = startY; y != endY; y += addValue)
	{
		for (int x = 0; x < m_SizeX; ++x)
		{
			auto pTile = GetTile(x, y);
			if (nullptr != pTile)
			{
				for (int diagIdx = 0; diagIdx < 2; ++diagIdx)
				{
					pTile->SetDistanceInfo(diagonalDirections[diagIdx], CalcDiagonalDistance((*pTile), diagonalDirections[diagIdx]));
				}
			}
		}
	}
}
