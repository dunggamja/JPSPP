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

	//������ ������ �ִ� ������ Ŭ����.
	ClearJumpPointInfo();


	//1.������ �밢��Ÿ���� ��ֹ����� �˻�. 
	//2.�밢���� ������ ���� Ÿ�� 2���� ����ִ��� �˻�. 
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
				return pTempTile->IsObstacle();  // ���� �밢�� Ÿ���� ��������.
			else
				return !pTempTile->IsObstacle(); // ���� ���� Ÿ���� �����ִ�.
		});

		//���Ǹ����̸�. ��������Ʈ ������ ����. 
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

