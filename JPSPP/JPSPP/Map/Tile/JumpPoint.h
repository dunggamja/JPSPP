#pragma once
#include "Global.h"


class JumpPoint
{
	//각각의 직선방향에 대한 진입 허용 여부를 저장합니다. 
	std::array<bool, Enum2Num(GAME::DIRECTIONS::STRAIGHT_COUNT)> m_IsAllowEntry = { false, };

public:
	JumpPoint() = default;

	//allowEntryInfo는 TOP, RIGHT, BOTTOM, LEFT 순으로 읽습니다.
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
	//해당방향에서 진입이 가능한지 체크합니다. 
	bool CheckAllowEntry(const GAME::DIRECTIONS from) const;
	void UpdateAllowEntry(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType);

	void Clear();

};
