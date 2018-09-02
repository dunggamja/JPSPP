#pragma once
#include "Global.h"
#include "JumpPoint.h"


class Tile 
{
public:
	using UniquePtr = std::unique_ptr<Tile>;
private:
	int m_PosX = 0;
	int m_PosY = 0;

	bool m_IsObstacle = false;
	std::array<int, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_END)> m_DistanceInfos = { 0, };
	

	JumpPoint m_JumpPointInfo;

public:
	Tile() = default;
	Tile(int posX, int posY, bool isObstacle
		, decltype(m_DistanceInfos) distanceInfos
		, decltype(m_JumpPointInfo) jumpPointInfo)
		: m_PosX(posX)
		, m_PosY(posY)
		, m_IsObstacle(isObstacle)
		, m_DistanceInfos(std::move(distanceInfos))
		, m_JumpPointInfo(std::move(jumpPointInfo))
	{}

public:
	inline bool IsObstacle() const { return m_IsObstacle; }
	inline void SetIsObstacle(const bool isObstacle) { m_IsObstacle = isObstacle; }

	inline void SetJumpPointAllowEntry(const GAME::DIRECTIONS direction, bool isAllowEntry) { m_JumpPointInfo.SetAllowEntry(direction, isAllowEntry); }
	inline bool GetJumpPointAllowEntry(const GAME::DIRECTIONS direction) const { return m_JumpPointInfo.GetAllowEntry(direction); }
	
	inline bool IsJumpPoint() const { return m_JumpPointInfo.IsJumpPoint(); }
	bool CheckJumpPointAllowEntry(const GAME::DIRECTIONS from) const { return m_JumpPointInfo.CheckAllowEntry(from); }


	inline void SetDistanceInfo(GAME::DIRECTIONS direction, int distance)
	{
		size_t idx = Enum2Num(direction);
		if (idx < 0 || m_DistanceInfos.size() <= idx) return;

		m_DistanceInfos[idx] = distance;
	}

	inline int GetDistanceInfo(GAME::DIRECTIONS direction) const
	{
		size_t idx = Enum2Num(direction);
		if (idx < 0 || m_DistanceInfos.size() <= idx) return 0;

		return m_DistanceInfos[idx];
	}


	inline int GetPosX() const { return m_PosX; }
	inline void SetPosX(int posX) { m_PosX = posX; }
	inline int GetPosY() const { return m_PosY; }
	inline void SetPosY(int posY) { m_PosY = posY; }



public:
	void Clear();
	void ClearJumpPointInfo();
	void ClearDistanceInfo();


	void UpdateJumpPointAllowEntry(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType);

};