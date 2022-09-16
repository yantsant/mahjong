#pragma once
#include "hand.h"
typedef std::pair<int, int> HAN_FU;
enum class HAND_NAME{

};
std::array<std::tuple<const char*, int, int>, 28> HANDS_NAME_COSTS = { 
	std::tuple{"pinfu "      , 1, 0},
	std::tuple{"ipeikou"     , 1, 0},
	std::tuple{"chiitoi"     , 2, 0},
	std::tuple{"sankantsu"   , 2, 0},
	std::tuple{"ryanpeikou"  , 3, 0},
	// yakuman close
	std::tuple{"kokushimusou", 13, 0}, //* +1
	std::tuple{"chuurenpooto", 13, 0}, //* +1
	std::tuple{"sanankou"    , 13, 0}, //  +1
	// open/close
	std::tuple{"tanyao"      , 1, 1},
	std::tuple{"yakuhai"     , 1, 1},
	std::tuple{"itsu"        , 2, 1},
	std::tuple{"sanshoku"    , 2, 1},
	std::tuple{"sanankou"    , 2, 1},
	std::tuple{"chanta"      , 2, 1},
	std::tuple{"sanshokudoku", 2, 2},
	std::tuple{"toitoi"      , 2, 2},
	std::tuple{"shousangen"  , 2, 2},
	std::tuple{"honrouto"    , 2, 2},
	std::tuple{"honitsu"     , 3, 2},
	std::tuple{"junchan"     , 3, 2},
	std::tuple{"chinitsu"    , 6, 5},
	// yakuman
	std::tuple{"sanankantsu" , 13, 13}, //  +1
	std::tuple{"ryuuiisou"   , 13, 13},
	std::tuple{"chinrouto"   , 13, 13},
	std::tuple{"tsuuiisou"   , 13, 13},
	std::tuple{"daisangen"   , 13, 13},
	std::tuple{"shousuushii" , 13, 13},
	std::tuple{"daisuushi"   , 26, 26}
};

class CalcCostHand
{
public:
	CalcCostHand(const std::vector < std::vector<SET>>& _hand) {
		hand = _hand;
	}
	~CalcCostHand() {};

private:
	std::vector < std::vector<SET>> hand;
};
