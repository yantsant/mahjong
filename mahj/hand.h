#pragma once
#include <iostream>
#include "set.h"
#include <array>
#include <memory>
#include <algorithm>

enum class SET_TYPE{
	CHI,
	PON,
	KAN,
	PAIR
};
enum class SET_STATE {
	CLOSE,
	OPEN
};

class Hand : private std::vector<Tile>
{
	typedef std::tuple<std::vector<Tile>, SET_TYPE, SET_STATE> SET;
public:
	Hand(const std::vector<int>& hand_tiles, const std::vector<int>& dora_poiters, int win_tile) {
		for (size_t i = 0; i < dora_poiters.size(); i++)		{
			dora_tiles.push_back(Tile::get_tile_by_index(Tile::get_next_tile_index(dora_poiters[i])));
		}
		for (size_t i = 0; i < hand_tiles.size(); i++)		{
			Tile new_tile = Tile::get_tile_by_index(hand_tiles[i]);
			new_tile.set_position(i);
			check_dora_tile(new_tile);
			//tiles.push_back(new_tile);
			this->push_back(new_tile);
		}

		divide_hand_to_sets();
	};

	~Hand() {
	};

private:
	std::vector<Tile> tiles;
	std::vector <SET> set;
	std::vector<Tile> dora_tiles;
	void check_dora_tile(Tile& tile) {
		for (const auto& dora : dora_tiles) {
			if (tile == dora)
				tile.set_as_dora(DORA::DORA);
		}
	};

	void divide_hand_to_sets();
	SET make_pon(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last) {
		SET new_set;
		std::get<1>(new_set) = SET_TYPE::PON;
		std::get<2>(new_set) = SET_STATE::CLOSE;
		auto it = first;
		do {
			std::get<0>(new_set).push_back(*it);
			it++;
		} while (!(it > last));
		tiles.erase(first, last+1);
		return new_set;
	}
};


void Hand::divide_hand_to_sets() {
	tiles = *this;
	sort(tiles.begin(), tiles.end(), Tile::compareTiles);
	std::vector<Tile> distinct_tiles = tiles;
	distinct_tiles.erase(unique(distinct_tiles.begin(), distinct_tiles.end()), distinct_tiles.end());
	for (const auto &tile : distinct_tiles){
		auto first = std::find(tiles.begin(), tiles.end(), tile);
		auto last = first;
		size_t count = 0;
		while (tile == *last) {
			count++;
			last++;
		};
		if (count == 3) {
			last--;
			set.push_back(make_pon(first, last));
			std::cout << count;
		}
	}
}