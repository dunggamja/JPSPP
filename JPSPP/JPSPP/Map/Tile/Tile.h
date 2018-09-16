#pragma once
#include "Global.h"
#include "JumpPoint.h"

class Map;


class Tile : std::enable_shared_from_this<Tile>
{
public:
	using UniquePtr = std::unique_ptr<Tile>;
	using SharedPtr = std::shared_ptr<Tile>;
	using WeakPtr = std::weak_ptr<Tile>;

private:
	std::weak_ptr<Map>	m_pMap;
	int					m_PosX = 0;
	int					m_PosY = 0;
	bool				m_IsObstacle = false;


	//점프포인트 관련 정보들 	
	bool																	m_IsJumpPoint = false;
	std::array<bool, static_cast<int>(GAME::DIRECTIONS::STRAIGHT_COUNT)>	m_IsJumpPointAccesFromDir = { false, };

	//프리프로세싱이 모두 끝나면 길찾기 정보는 이쪽에 저장된다. (8방향)
	std::array<int, static_cast<int>(GAME::DIRECTIONS::DIRECTIONS_END)>		m_DistanceInfos = { 0, };

public:
	Tile(std::weak_ptr<Map> pMap
		, int posX, int posY)
		: m_pMap(pMap)
		, m_PosX(posX)
		, m_PosY(posY)
	{}

	~Tile();

public:
	inline bool IsObstacle() const { return m_IsObstacle; }
	inline void SetIsObstacle(const bool isObstacle) { m_IsObstacle = isObstacle; }

	inline bool IsJumpPoint() const { return m_IsJumpPoint; }
	inline bool IsJumpPointAcessFrom(GAME::DIRECTIONS fromDir)  const 
	{
		if (!IsStraightDirection(fromDir)) 
			return false;

		auto idx = Enum2Num(fromDir);
		return idx < m_IsJumpPointAccesFromDir.size() ? m_IsJumpPointAccesFromDir[idx] : false;
	}
	

	inline void SetDistanceInfo(GAME::DIRECTIONS direction, int distance)
	{
		auto idx = Enum2Num(direction);
		if (m_DistanceInfos.size() <= idx) return;

		m_DistanceInfos[idx] = distance;
	}

	inline int GetDistanceInfo(GAME::DIRECTIONS direction) const
	{
		auto idx = Enum2Num(direction);
		if (m_DistanceInfos.size() <= idx) return 0;

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

	void UpdateJumpPoint();
};