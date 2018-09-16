#pragma once
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <memory>
#include <tuple>
#include <array>
#include <initializer_list>
#include <ppl.h>
#include <functional>
#include <numeric>
#include <random>
#include <intrin.h>
#include <chrono>
#include "DefineGame.h"


#define IS_NULL(p)			(nullptr == p)
#define IS_NOT_NULL(p)		(nullptr != p)


template <typename T>
constexpr auto Enum2Num(T t){ return std::underlying_type_t<T>(t); }

template <size_t T>
struct Num2Struct { };




inline constexpr bool IsValidDirection(const GAME::DIRECTIONS direction)
{
	return (GAME::DIRECTIONS::DIRECTION_BEGIN <= direction) && (direction < GAME::DIRECTIONS::DIRECTIONS_END);
}


inline constexpr bool IsStraightDirection(const GAME::DIRECTIONS direction)
{
	return (GAME::DIRECTIONS::STRAIGHT_BEGIN <= direction) && (direction < GAME::DIRECTIONS::STRAIGHT_END);
}

inline constexpr bool IsDiagonalDirection(const GAME::DIRECTIONS direction)
{
	return (GAME::DIRECTIONS::DIAGONAL_BEGIN <= direction) && (direction < GAME::DIRECTIONS::DIAGONAL_END);
}

constexpr GAME::DIRECTIONS GetReverseDirection(const GAME::DIRECTIONS direction)
{
	switch (direction)
	{
	case GAME::DIRECTIONS::TOP:				return GAME::DIRECTIONS::BOTTOM;
	case GAME::DIRECTIONS::RIGHT:			return GAME::DIRECTIONS::LEFT;
	case GAME::DIRECTIONS::BOTTOM:			return GAME::DIRECTIONS::TOP;
	case GAME::DIRECTIONS::LEFT:			return GAME::DIRECTIONS::RIGHT;
	case GAME::DIRECTIONS::RIGHTTOP:		return GAME::DIRECTIONS::LEFTBOTTOM;
	case GAME::DIRECTIONS::LEFTTOP:			return GAME::DIRECTIONS::RIGHTBOTTOM;
	case GAME::DIRECTIONS::RIGHTBOTTOM:		return GAME::DIRECTIONS::LEFTTOP;
	case GAME::DIRECTIONS::LEFTBOTTOM:		return GAME::DIRECTIONS::RIGHTTOP;
	}
	return GAME::DIRECTIONS::DIRECTIONS_END;
}

extern std::vector<GAME::DIRECTIONS> VEC_ALL_DIRECTION;
extern std::vector<GAME::DIRECTIONS> VEC_LOOK_UP_TABLE[static_cast<int>(GAME::DIRECTIONS::DIRECTIONS_COUNT)];


inline const std::vector<GAME::DIRECTIONS>& GetLookUpDirs(const GAME::DIRECTIONS direction)
{
	if (IsValidDirection(direction))
		return VEC_LOOK_UP_TABLE[Enum2Num(direction)];
	else
		return VEC_ALL_DIRECTION;
}

constexpr GAME::POS_INFO GetAdjacentDir(const GAME::DIRECTIONS direction)
{
	int posX = 0;
	int posY = 0;

	switch (direction)
	{
	case GAME::DIRECTIONS::TOP:
		posY -= 1;
		break;
	case GAME::DIRECTIONS::RIGHT:
		posX += 1;
		break;
	case GAME::DIRECTIONS::BOTTOM:
		posY += 1;
		break;
	case GAME::DIRECTIONS::LEFT:
		posX -= 1;
		break;
	case GAME::DIRECTIONS::RIGHTTOP:
		posX += 1;
		posY -= 1;
		break;
	case GAME::DIRECTIONS::LEFTTOP:
		posX -= 1;
		posY -= 1;
		break;
	case GAME::DIRECTIONS::RIGHTBOTTOM:
		posX += 1;
		posY += 1;
		break;
	case GAME::DIRECTIONS::LEFTBOTTOM:
		posX -= 1;
		posY += 1;
		break;
	}
	return std::make_tuple(posX, posY);
}

constexpr GAME::POS_INFO GetNeighborPos(int posX, int posY, const GAME::DIRECTIONS direction)
{
	switch(direction)
	{
	case GAME::DIRECTIONS::TOP:
		posY -= 1;
		break;
	case GAME::DIRECTIONS::RIGHT:
		posX += 1;
		break;
	case GAME::DIRECTIONS::BOTTOM:
		posY += 1;
		break;
	case GAME::DIRECTIONS::LEFT:
		posX -= 1;
		break;
	case GAME::DIRECTIONS::RIGHTTOP:
		posX += 1;
		posY -= 1;
		break;
	case GAME::DIRECTIONS::LEFTTOP:
		posX -= 1;
		posY -= 1;
		break;
	case GAME::DIRECTIONS::RIGHTBOTTOM:
		posX += 1;
		posY += 1;
		break;
	case GAME::DIRECTIONS::LEFTBOTTOM:
		posX -= 1;
		posY += 1;
		break;
	}

	return std::make_tuple(posX, posY);
}