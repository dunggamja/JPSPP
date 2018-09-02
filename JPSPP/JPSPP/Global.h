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

#include "DefineGame.h"





template <typename T>
constexpr size_t Enum2Num(T t){ return std::underlying_type_t<T>(t); }

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
