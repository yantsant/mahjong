#pragma once
#include <vector>
#include <tuple>
#include <string>
#include <assert.h>
enum class SUIT{
	UNDEF = 0,
	PIN = 1,
	SOU = 2,
	MAN = 3,
	WIND = 4,
	DRAGON = 5,
};



int get_suit_range(SUIT suit) {
	switch (suit)	{
	case SUIT::UNDEF:
		return 0;
		break;
	case SUIT::PIN:
		return 9;
		break;
	case SUIT::SOU:
		return 9;
		break;
	case SUIT::MAN:
		return 9;
		break;
	case SUIT::WIND:
		return 4;
		break;
	case SUIT::DRAGON:
		return 3;
		break;
	default:
		return 0;
		break;
	}
	return 0;
}

std::string tile_captions[38] = {
		"pin-1", "pin-2", "pin-3", "pin-4", "pin-5", "pin-6", "pin-7", "pin-8", "pin-9", "pin-5-red",//  0- 9
		"sou-1", "sou-2", "sou-3", "sou-4", "sou-5", "sou-6", "sou-7", "sou-8", "sou-9", "sou-5-red",// 10-19
		"man-1", "man-2", "man-3", "man-4", "man-5", "man-6", "man-7", "man-8", "man-9", "man-5-red",// 20-29
		"hon-east", "hon-south", "hon-west", "hon-north",											 // 30-33
		"hon-red", "hon-white", "hon-green",													     // 34-36
		"back"                                                                                       // 37
};	

std::pair<SUIT, int> get_suit_num_by_index(int index) {
	assert((index > -1 && index < 37) && "wrong the tile index");
	int suitnum = index / 10 + 1;
	int num = index % 10;
	if (index > 33) {
		suitnum++;
		num -= 4;
	}
	return std::make_pair(SUIT(suitnum), num);
}

int get_index_by_suit_num(std::pair<SUIT, int> suit_num) {
	int start_suit_index = 0;
	switch (suit_num.first) {
	case SUIT::UNDEF:
		start_suit_index = 0;
		break;
	case SUIT::PIN:
		start_suit_index = 0;
		break;
	case SUIT::SOU:
		start_suit_index = 10;
		break;
	case SUIT::MAN:
		start_suit_index = 20;
		break;
	case SUIT::WIND:
		start_suit_index = 30;
		break;
	case SUIT::DRAGON:
		start_suit_index = 34;
		break;
	default:
		start_suit_index = 0;
		break;
	}
	return start_suit_index + suit_num.second;
}

enum class TYPE{
	UNDEF = 0,
	SIMPLE = 1,
	TERMINAL = 2,
	HONOR = 3
};

enum class DORA{
	RED_FIVE,
	DORA
};

enum class STATE {
	OPEN,
	CLOSE
};

//                 <idx, num in suit,   name,        suit, type, dora array,        state, pos>
typedef  std::tuple<int,         int,   std::string, SUIT, TYPE, std::vector<DORA>, STATE, int> TUPLE_TILE;

class Tile : private TUPLE_TILE
{
public:
	Tile(int idx, int num_in_suit, std::string name, SUIT suit, TYPE type) {
		std::get<0>(*this) = idx;
		std::get<1>(*this) = num_in_suit;
		std::get<2>(*this) = name;
		std::get<3>(*this) = suit;
		std::get<4>(*this) = type;
		//std::get<5>(*this).push_back( dora);
		std::get<6>(*this) = STATE::CLOSE;
		std::get<7>(*this) = -1;
	}
	int  get_index()         const { return std::get<0>(*this); };
	int  get_num_in_suit()   const { return std::get<1>(*this); };
	SUIT get_suit()          const { return std::get<3>(*this); };
	std::string get_name()   const { return std::get<2>(*this); };
	TYPE get_type()          const { return std::get<4>(*this); };
	size_t get_dora_count()  const { return std::get<5>(*this).size(); };
	//int  get_position()      const { return std::get<0>(*this); };
	void  set_position(int pos)    { std::get<7>(*this) = pos; };

	void set_as_terminal()           { std::get<4>(*this) = TYPE::TERMINAL;	}
	void set_as_honor()              { std::get<4>(*this) = TYPE::HONOR;	}
	void set_as_dora(DORA dora_type) { std::get<5>(*this).push_back(dora_type); };// std::get<5>(*this) = dora_type;

	friend static bool operator == (const Tile& lhs, const Tile& rhs) {
		if(lhs.get_suit() == rhs.get_suit())
			if(lhs.get_num_in_suit() == rhs.get_num_in_suit())
				return true;
		return false;
	}

	friend static bool operator > (const Tile& lhs, const Tile& rhs) {
		return lhs.get_index() - rhs.get_index();
	}
	friend static bool operator < (const Tile& lhs, const Tile& rhs) {
		return rhs.get_index() - lhs.get_index();
	}
	static Tile get_tile_by_index  (int index);
	static int  get_next_tile_index(int index);
	static bool compareTiles(const Tile& lhs, const Tile& rhs);
	~Tile() {};
private:
	Tile() { 
		std::vector<DORA> dora;
		static_cast<TUPLE_TILE>(*this) = {37, 0, tile_captions[37], SUIT::UNDEF, TYPE::UNDEF,  dora, STATE::CLOSE, -1 }; };
};


bool Tile::compareTiles(const Tile& lhs, const Tile& rhs){
	return std::get<0>(lhs) < std::get<0>(rhs);
}

Tile Tile::get_tile_by_index(int index) {
	std::pair<SUIT, int> suit_num = get_suit_num_by_index(index);
	int num = suit_num.second + 1;
	Tile tile ( index, num, tile_captions[index], suit_num.first, TYPE::SIMPLE );

	if (index > 29)
		tile.set_as_honor(); 
	else {
		if (num == 1 || num == 9)
			tile.set_as_terminal();
		else if (num == 10) {
			std::get<1>(tile) = 5;
			std::get<0>(tile) = get_index_by_suit_num(std::make_pair(suit_num.first, num));
			tile.set_as_dora(DORA::RED_FIVE);
		}
	}

	return tile;
}


int Tile::get_next_tile_index(int index) {
	std::pair<SUIT, int> suit_num = get_suit_num_by_index(index);
	if (suit_num.second == 10) { // red five
		suit_num.second = 5;    // just five
	}
	int range    = get_suit_range(suit_num.first);
	suit_num.second = (index+1)%range;
	return get_index_by_suit_num(suit_num)-1;
}