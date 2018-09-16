//#pragma once
//#include "Global.h"
//
//
//class JumpPoint
//{
//	//각각의 직선방향에 대한 진입 허용 여부를 저장합니다. 
//	std::array<bool, Enum2Num(GAME::DIRECTIONS::STRAIGHT_COUNT)> m_AccessInfos = { false, };
//
//public:
//	JumpPoint() = default;
//
//	//allowEntryInfo는 TOP, RIGHT, BOTTOM, LEFT 순으로 읽습니다.
//	JumpPoint(std::initializer_list<bool> allowEntryInfo);
//
//
//public:
//	inline void SetAccessInfo(const GAME::DIRECTIONS fromDir, bool isAccesible)
//	{
//		if (!IsStraightDirection(fromDir))
//			return;
//
//		m_AccessInfos[Enum2Num(fromDir)] = isAccesible;
//	}
//
//	inline bool GetAccessInfo(const GAME::DIRECTIONS fromDir) const
//	{
//		if (!IsStraightDirection(fromDir))
//			return false;
//
//		return m_AccessInfos[Enum2Num(fromDir)];
//	}
//
//
//	inline bool IsJumpPoint() const
//	{
//		return std::any_of(m_AccessInfos.cbegin(), m_AccessInfos.cend()
//			, [](const std::remove_reference_t<decltype(m_AccessInfos)>::value_type& value)->bool
//		{
//			return value;
//		});
//	}
//
//
//public:
//	//해당방향에서 진입이 가능한지 체크합니다. 
//	bool CheckAccessFrom(const GAME::DIRECTIONS fromDir) const;
//	void UpdateAccessInfo(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType);
//
//	void Clear();
//
//};
