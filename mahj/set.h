#pragma once
#include <vector>
#include <tuple>
#include <string>
#include <assert.h>
enum class SUIT
{
	UNDEF = 0,
	PIN = 1,
	SOU = 2,
	MAN = 3,
	WIND = 4,
	DRAGON = 5
};
std::string tile_captions[37] = {
		"pin-1", "pin-2", "pin-3", "pin-4", "pin-5", "pin-6", "pin-7", "pin-8", "pin-9", "pin-5-red",//  0- 9
		"sou-1", "sou-2", "sou-3", "sou-4", "sou-5", "sou-6", "sou-7", "sou-8", "sou-9", "sou-5-red",// 10-19
		"man-1", "man-2", "man-3", "man-4", "man-5", "man-6", "man-7", "man-8", "man-9", "man-5-red",// 20-29
		"hon-east", "hon-south", "hon-west", "hon-north",											 // 30-33
		"hon-red", "hon-white", "hon-green"};														 // 34-36

enum class TYPE
{
	UNDEF = 0,
	SIMPLE = 1,
	TERMINAL = 2,
	HONOR = 3
};

enum class DORA
{
	NOT_DORA,
	RED_FIVE,
	DORA,
	URA_DORA
};
//                 <idx,           num in suit,   name,        suit, type, dora>
typedef  std::tuple<int, int, std::string, SUIT, TYPE, DORA> TUPLE_TILE;

class TILE : private TUPLE_TILE
{
public:
	TILE(int idx, int num_in_suit, std::string name, SUIT suit, TYPE type, DORA dora) {
		std::get<0>(*this) = idx;
		std::get<1>(*this) = num_in_suit;
		std::get<2>(*this) = name;
		std::get<3>(*this) = suit;
		std::get<4>(*this) = type;
		std::get<5>(*this) = dora;
	}

	void set_as_terminal() {
		std::get<4>(*this) = TYPE::TERMINAL;
	}

	void set_as_honor() {
		std::get<4>(*this) = TYPE::HONOR;
	}

	void set_as_dora(DORA dora_type) {
		std::get<5>(*this) = dora_type;
	}

	~TILE() {};
private:
	TILE() { static_cast<TUPLE_TILE>(*this) = { 0, 0, "", SUIT::UNDEF, TYPE::UNDEF, DORA::NOT_DORA }; };
};


TILE get_tile_by_index(int index) {
	assert((index > -1 && index < 37) && "wrong the tile index");


	int num     = index % 10 + 1;
	int suitnum = index / 10 + 1;
	if (index > 33) {
		num -= 4;
		suitnum++;
	}

	TILE tile = { index, num,tile_captions[index], SUIT(suitnum), TYPE::SIMPLE, DORA::NOT_DORA };

	if (index > 29)
		tile.set_as_honor(); //std::get<4>(tile) = TYPE::HONOR;
	else {
		if (num == 1 || num == 9)
			tile.set_as_terminal();// std::get<4>(tile) = TYPE::TERMINAL;
		else if (num == 10)
			tile.set_as_dora(DORA::RED_FIVE);// std::get<5>(tile) = DORA::RED_FIVE;
	}

	return tile;
}