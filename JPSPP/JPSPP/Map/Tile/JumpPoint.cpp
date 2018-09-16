//#include "JumpPoint.h"
//
//
////allowEntryInfo는 TOP, RIGHT, BOTTOM, LEFT 순으로 읽습니다.
//JumpPoint::JumpPoint(std::initializer_list<bool> accessInfos)
//{
//	auto iterS = accessInfos.begin();
//	auto iterE = accessInfos.end();
//
//	for (int i = 0; i < m_AccessInfos.size() && iterS != iterE; ++i, ++iterS)
//	{
//		m_AccessInfos[i] = (*iterS);
//	}
//}
//
//
//bool JumpPoint::CheckAccessFrom(const GAME::DIRECTIONS from) const
//{
//	if (!IsValidDirection(from))
//		return false;
//
//	auto lookUpTable = GetLookupTable(from);
//	return std::any_of(lookUpTable.begin(), lookUpTable.end()
//		, [&](const std::remove_reference_t<decltype(lookUpTable)>::value_type& lookUpDirection)->bool
//	{
//		return GetAccessInfo(lookUpDirection);
//	});
//}
//
//
//void JumpPoint::UpdateAccessInfo(const std::array<GAME::TILE_TYPE, Enum2Num(GAME::DIRECTIONS::DIRECTIONS_COUNT)>& neighborTileType)
//{
//	std::array<bool, Enum2Num(GAME::DIRECTIONS::STRAIGHT_COUNT)> allowEntry = { false, };
//
//	auto checkJumpPointCondition_FromDiagonalObstacle = [&](const GAME::DIRECTIONS fromDir)->bool 
//	{
//		if (!IsDiagonalDirection(fromDir))
//			return false;
//
//		auto lookUpTable = GetLookupTable(fromDir);
//		return std::all_of(lookUpTable.begin(), lookUpTable.end()
//			, [&](const std::remove_reference_t<decltype(lookUpTable)>::value_type& lookUpDir)->bool 
//		{
//			if (IsDiagonalDirection(lookUpDir))
//				return neighborTileType[Enum2Num(lookUpDir)] == GAME::TILE_TYPE::OBSTACLE;
//			else
//				return neighborTileType[Enum2Num(lookUpDir)] == GAME::TILE_TYPE::EMPTY;
//		});
//	};
//
//	auto CheckDiagonal_StraightEntryAllow = [&](GAME::DIRECTIONS fromDir)->bool 
//	{
//		const auto lookUpDirections = GetLookupTable(fromDir);
//
//		auto iterS = lookUpDirections.begin();
//		auto iterE = lookUpDirections.end();
//		for (; iterS != iterE; ++iterS)
//		{
//			auto a = (*iterS);
//			int at = (int)a;
//		}
//
//		return std::any_of(lookUpDirections.begin(), lookUpDirections.end(),
//			[&](const std::remove_reference_t<decltype(lookUpDirections)>::value_type& lookUpValue)->bool
//		{
//			int a = (int)lookUpValue;
//			if (IsDiagonalDirection(lookUpValue) && neighborTileType[Enum2Num(lookUpValue)] == GAME::TILE_TYPE::OBSTACLE)
//				return checkJumpPointCondition_FromDiagonalObstacle(lookUpValue);
//			else
//				return false;
//		});
//	};
//
//
//	//1.직선방향으로 비어있는 타일 검사. 
//	//2.비어잇는 타일과 인접한 대각선타일이 장애물인지 검사. 
//	//3.대각선과 인접한 직선 타일 2개가 비어있는지 검사. 
//	for (size_t idx = Enum2Num(GAME::DIRECTIONS::STRAIGHT_BEGIN); idx < Enum2Num(GAME::DIRECTIONS::STRAIGHT_END); ++idx)
//	{
//		if (GAME::TILE_TYPE::EMPTY == neighborTileType[idx])
//		{
//			allowEntry[idx] = CheckDiagonal_StraightEntryAllow(static_cast<GAME::DIRECTIONS>(idx));
//		}
//	}
//
//	m_AccessInfos = allowEntry;
//}
//
//void JumpPoint::Clear()
//{
//	for (auto& allowEntry : m_AccessInfos)
//		allowEntry = false;
//}
