#pragma once
#include "Global.h"


class JumpPoint
{
	//������ �������⿡ ���� ���� ��� ���θ� �����մϴ�. 
	std::array<bool, Enum2Num(GAME::DIRECTIONS::STRAIGHT_COUNT)> m_IsAllowEntry = { false, };

public:
	JumpPoint() = default;

	//allowEntryInfo�� TOP, RIGHT, BOTTOM, LEFT ������ �н��ϴ�.
	JumpPoint(std::initializer_list<bool> allowEntryInfo);


public:
	inline void SetAllowEntry(const GAME::DIRECTIONS direction, bool isAllowEntry)
	{
		size_t idx = Enum2Num(direction) - Enum2Num(GAME::DIRECTIONS::STRAIGHT_BEGIN);
		if (idx < 0 || m_IsAllowEntry.size() <= idx) return;

		m_IsAllowEntry[idx] = isAllowEntry;
	}

	inline bool GetAllowEntry(const GAME::DIRECTIONS direction) const
	{
		size_t idx = Enum2Num(direction) - Enum2Num(GAME::DIRECTIONS::STRAIGHT_BEGIN);
		if (idx < 0 || m_IsAllowEntry.size() <= idx) return false;

		return m_IsAllowEntry[idx];
	}


	inline bool IsJumpPoint() const
	{
		return std::any_of(m_IsAllowEntry.cbegin(), m_IsAllowEntry.cend()
			, [](const std::remove_reference_t<decltype(m_IsAllowEntry)>::value_type& value)->bool
		{
			return value;
		});
	}


public:
	//�ش���⿡�� ������ �������� üũ�մϴ�. 
	bool CheckAllowEntry(const GAME::DIRECTIONS from) const;
	void UpdateAllowEntry(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType);

	void Clear();

};
