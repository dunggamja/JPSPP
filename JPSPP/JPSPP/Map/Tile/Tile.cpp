#include "Tile.h"

void Tile::Clear()
{
	m_PosX = m_PosY = 0;
	m_IsObstacle = false;

	ClearDistanceInfo();
	ClearJumpPointInfo();
}

void Tile::ClearJumpPointInfo()
{
	m_JumpPointInfo.Clear();
}

void Tile::ClearDistanceInfo()
{
	for (auto& distanceValue : m_DistanceInfos)
		distanceValue = 0;
}


void Tile::UpdateJumpPointAllowEntry(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType)
{
	m_JumpPointInfo.UpdateAllowEntry(neighborTileType);
}
