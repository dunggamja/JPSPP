#include "Tile.h"
#include "../Map.h"

Tile::~Tile() = default;


void Tile::Clear()
{
	m_PosX = m_PosY = 0;
	m_IsObstacle = false;

	ClearDistanceInfo();
	ClearJumpPointInfo();
}

void Tile::ClearJumpPointInfo()
{
	m_IsJumpPoint = false;
	m_IsJumpPointAccesFromDir = { false, false, false, false};
}

void Tile::ClearDistanceInfo()
{
	for (auto& distanceValue : m_DistanceInfos)
		distanceValue = 0;
}


void Tile::UpdateJumpPoint()
{
	
	auto pMap = m_pMap.lock();
	if (!pMap) return;

	//기존에 가지고 있던 정보는 클리어.
	ClearJumpPointInfo();


	//1.인접한 대각선타일이 장애물인지 검사. 
	//2.대각선과 인접한 직선 타일 2개가 비어있는지 검사. 
	for (int i = static_cast<int>(GAME::DIRECTIONS::DIAGONAL_BEGIN); i < static_cast<int>(GAME::DIRECTIONS::DIAGONAL_END); ++i)
	{
		auto& lookUp = GetLookUpDirs(static_cast<GAME::DIRECTIONS>(i));
		bool checkCondition = std::all_of(lookUp.cbegin(), lookUp.cend()
			, [&](const GAME::DIRECTIONS& dir)->bool
		{
			int tempX = 0, tempY = 0;
			std::tie(tempX, tempY) = GetNeighborPos(m_PosX, m_PosY, dir);
			auto pTempTile = pMap->GetTile(tempX, tempY);


			if (nullptr == pTempTile) return false;

			
			if (IsDiagonalDirection(dir))
				return pTempTile->IsObstacle();  // 인접 대각선 타일은 갈수없다.
			else
				return !pTempTile->IsObstacle(); // 인접 직선 타일은 갈수있다.
		});

		//조건만족이면. 점프포인트 정보를 갱신. 
		if (checkCondition)
		{
			for (auto& dir : lookUp)
			{
				if (IsStraightDirection(dir))
					m_IsJumpPointAccesFromDir[(int)dir] = true;
			}
		}
	}

	m_IsJumpPoint = std::any_of(m_IsJumpPointAccesFromDir.cbegin(), m_IsJumpPointAccesFromDir.cend(), [](const bool& value)->bool {return value; });
}

