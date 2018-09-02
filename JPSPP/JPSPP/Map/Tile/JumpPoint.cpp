#include "JumpPoint.h"


//allowEntryInfo�� TOP, RIGHT, BOTTOM, LEFT ������ �н��ϴ�.
JumpPoint::JumpPoint(std::initializer_list<bool> allowEntryInfo)
{
	auto iterS = allowEntryInfo.begin();
	auto iterE = allowEntryInfo.end();

	for (int i = 0; i < m_IsAllowEntry.size() && iterS != iterE; ++i, ++iterS)
	{
		m_IsAllowEntry[i] = (*iterS);
	}
}


bool JumpPoint::CheckAllowEntry(const GAME::DIRECTIONS from) const
{
	if (from < GAME::DIRECTIONS::DIRECTION_BEGIN || GAME::DIRECTIONS::DIRECTIONS_END <= from)
		return false;

	size_t idx = Enum2Num(from);

	return std::any_of(GAME::DIR_LOOKUP_TABLES[idx].begin(), GAME::DIR_LOOKUP_TABLES[idx].end()
		, [&](const std::remove_reference_t<decltype(GAME::DIR_LOOKUP_TABLES[idx])>::value_type& lookUpDirection)->bool
	{
		return GetAllowEntry(lookUpDirection);
	});
}


void JumpPoint::UpdateAllowEntry(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType)
{
	std::array<bool, Enum2Num(GAME::DIRECTIONS::STRAIGHT_COUNT)> allowEntry = { false, };

	auto CheckDiagonal_StraightEntryAllow = [&](GAME::DIRECTIONS dir)->bool 
	{
		for(size_t idx = Enum2Num(GAME::DIRECTIONS::DIAGONAL_BEGIN); idx < Enum2Num(GAME::DIRECTIONS::DIAGONAL_END); ++idx)
		{
			if (GAME::TILE_TYPE::OBSTACLE == neighborTileType[idx])
			{
				if (std::find(GAME::DIR_LOOKUP_TABLES[idx].begin(), GAME::DIR_LOOKUP_TABLES[idx].end(), dir) != GAME::DIR_LOOKUP_TABLES[idx].end())
					return true;
			}
		}
		return false;
	};


	//1.�������� Ÿ���� �̵������� Ÿ������ üũ�Ѵ�.
	//2.�̵������� ����Ÿ�ϰ� ������ �밢�� Ÿ���� �̵��Ұ����� Ÿ���̶�� ��������Ʈ�� ���� 
	for (size_t idx = Enum2Num(GAME::DIRECTIONS::STRAIGHT_BEGIN); idx < Enum2Num(GAME::DIRECTIONS::STRAIGHT_END); ++idx)
	{
		if (GAME::TILE_TYPE::EMPTY == neighborTileType[idx])
		{
			//STRAIGHT_BEGIN =0 �̹Ƿ� ��� ���ʿ�� ����.
			size_t allowArrayIdx = idx - Enum2Num(GAME::DIRECTIONS::STRAIGHT_BEGIN);
		
			allowEntry[allowArrayIdx] = CheckDiagonal_StraightEntryAllow(static_cast<GAME::DIRECTIONS>(idx));
		}
	}

	m_IsAllowEntry = allowEntry;
}

void JumpPoint::Clear()
{
	for (auto& allowEntry : m_IsAllowEntry)
		allowEntry = false;
}
