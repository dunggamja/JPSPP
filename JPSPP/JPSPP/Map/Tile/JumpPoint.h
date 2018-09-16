//#pragma once
//#include "Global.h"
//
//
//class JumpPoint
//{
//	//������ �������⿡ ���� ���� ��� ���θ� �����մϴ�. 
//	std::array<bool, Enum2Num(GAME::DIRECTIONS::STRAIGHT_COUNT)> m_AccessInfos = { false, };
//
//public:
//	JumpPoint() = default;
//
//	//allowEntryInfo�� TOP, RIGHT, BOTTOM, LEFT ������ �н��ϴ�.
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
//	//�ش���⿡�� ������ �������� üũ�մϴ�. 
//	bool CheckAccessFrom(const GAME::DIRECTIONS fromDir) const;
//	void UpdateAccessInfo(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType);
//
//	void Clear();
//
//};
