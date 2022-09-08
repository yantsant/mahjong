#pragma once
#include "set.h"

class Hand : private std::vector<Tile>
{
public:
	Hand(const std::vector<int>& hand_tiles, const std::vector<int>& dora_poiters, int win_tile) {
		for (size_t i = 0; i < dora_poiters.size(); i++)		{
			dora_tiles.push_back(Tile::get_tile_by_index(Tile::get_next_tile_index(dora_poiters[i])));
		}
		for (size_t i = 0; i < hand_tiles.size(); i++)		{
			Tile new_tile = Tile::get_tile_by_index(hand_tiles[i]);
			check_dora_tile(new_tile);
			this->push_back(new_tile);
		}
	};

	~Hand() {
	};

private:
	std::vector<Tile> dora_tiles;
	void check_dora_tile(Tile& tile) {
		for (const auto& dora : dora_tiles)		{
			if (tile == dora)
				tile.set_as_dora(DORA::DORA);
				//return true;
		}
	}
};