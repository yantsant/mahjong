#pragma once
#include <iostream>
#include "tile.h"
#include <array>
#include <memory>
#include <algorithm>

enum class SET_TYPE{
	CHI,
	PON,
	KAN,
	PAIR
};

class Hand : private std::vector<Tile>
{
	typedef std::tuple<std::vector<Tile>, SET_TYPE, STATE> SET;
public:
	Hand(const std::vector<int>& hand_tiles, const std::vector<int>& dora_poiters, int win_tile) {
		for (size_t i = 0; i < dora_poiters.size(); i++)		{
			dora_tiles.push_back(Tile::get_tile_by_index(Tile::get_next_tile_index(dora_poiters[i])));
		}
		for (int i = 0; i < hand_tiles.size(); i++)		{
			Tile new_tile = Tile::get_tile_by_index(hand_tiles[i]);
			new_tile.set_position(i);
			check_dora_tile(new_tile);
			//tiles.push_back(new_tile);
			this->push_back(new_tile);
		}

		sort(this->begin(), this->end(), Tile::compareTiles);
		divide_hand_to_suits();
		for (auto suit : suits) {
			divide_suit_to_sets(suit);
		}
	};

	~Hand() {
	};

private:
	std::vector<std::vector<Tile>> suits;
	std::vector<Tile> tiles;
	std::vector <SET> sets;
	std::vector<Tile> dora_tiles;
	void check_dora_tile(Tile& tile) {
		for (const auto& dora : dora_tiles) {
			if (tile == dora)
				tile.set_as_dora(DORA::DORA);
		}
	};
	void divide_hand_to_suits();
	bool divide_suit_to_sets(std::vector<Tile> suit);
	bool make_par(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
	bool make_pon(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
	bool make_chi(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
	bool make_kan(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
};

void Hand::divide_hand_to_suits() {
	tiles = *this;
	std::vector<Tile> suit_set;
	auto it = tiles.begin();
	SUIT suit = it->get_suit();
	bool suit_complite = false;
	while (tiles.size() > 0) {
		if (it->get_suit() == suit) {
			suit_set.push_back(*it);
			tiles.erase(tiles.begin());
			it = tiles.begin();
			if (it < tiles.end() - 1)
				++it;
			else suit_complite = true;
		}
		else suit_complite = true;
		if (suit_complite) {
			suit = it->get_suit();
			suits.push_back(suit_set);
			if (suit_set.size() == tiles.size()) {
				tiles.clear();
				break;
			}
			suit_set.clear();
			suit_complite = false;
		};
	};
};


bool Hand::make_par(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set) {
	auto it = first;
	if (!(*it == *(it + 1))) return false;
	std::get<1>(new_set) = SET_TYPE::PAIR;
	std::get<2>(new_set) = STATE::CLOSE;
	do {
		std::get<0>(new_set).push_back(*it);
		it++;
	} while (!(it > last));
	tiles.erase(first, last + 1);
	return true;
}

bool Hand::make_pon(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set) {
	auto it = first;
	if (!(*it == *(it + 1) && *it == *(it + 2))) return false;
	std::get<1>(new_set) = SET_TYPE::PON;
	std::get<2>(new_set) = STATE::CLOSE;
	do {
		std::get<0>(new_set).push_back(*it);
		if (it->get_state() == STATE::OPEN)
			std::get<2>(new_set) = STATE::OPEN;
		it++;
	} while (!(it > last));
	tiles.erase(first, last + 1);
	return true;
}

bool Hand::make_kan(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set) {
	auto it = first;
	if (!(*it == *(it + 1) && *it == *(it + 2) && *it == *(it + 3))) return false;
	std::get<1>(new_set) = SET_TYPE::KAN;
	std::get<2>(new_set) = STATE::CLOSE;
	do {
		std::get<0>(new_set).push_back(*it);
		if (it->get_state() == STATE::OPEN)
			std::get<2>(new_set) = STATE::OPEN;
		it++;
	} while (!(it > last));
	tiles.erase(first, last + 1);
	return true;
}

bool Hand::make_chi(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set) {
	auto it = first;
	int nums[3] = { it->get_num_in_suit(), (it+1)->get_num_in_suit(), (it+2)->get_num_in_suit()};
	if (!(nums[1] == nums[0]+1 && nums[2] == nums[0]+1)) return false;
	std::get<1>(new_set) = SET_TYPE::CHI;
	std::get<2>(new_set) = STATE::CLOSE;
	do {
		std::get<0>(new_set).push_back(*it);
		if (it->get_state() == STATE::OPEN)
			std::get<2>(new_set) = STATE::OPEN;
		it++;
	} while (!(it > last));
	tiles.erase(first, last + 1);
	return true;
}

bool Hand::divide_suit_to_sets(std::vector<Tile> suit) {
	if (suit.size() == 2) { // only a pair
		SET set;
		if (make_par(suit.begin(), suit.end(), set)) {
			sets.push_back(set);
		}
	}	else if (suit.size() == 3) { // may be chi or pon
		SET set;
		if (make_pon(suit.begin(), suit.end(), set)) {
			sets.push_back(set);
		}
		else if (make_chi(suit.begin(), suit.end(), set)) {
			sets.push_back(set);
		} else { // check 13 orphans ?
			return false;
		}
	}	else if (suit.size() == 4) { // two pairs (seven pairs in hand) or kan?
		SET set;
		if (make_kan(suit.begin(), suit.end(), set)) {
			sets.push_back(set);
		}
		else {
			SET set1, set2;
			if (make_par(suit.begin(), suit.begin() + 2, set1) && make_par(suit.begin() + 2, suit.end(), set2)) {
				sets.push_back(set1);
				sets.push_back(set2);
			} else return false;
		}
	}	else {
		for (int i = 4; i >= 2; i--) {
			std::vector<Tile> suit_cut(suit.begin(), suit.begin()+i);
			divide_suit_to_sets(suit_cut);
		}
	}

	//tiles = suit;
	//sort(tiles.begin(), tiles.end(), Tile::compareTiles);
	//std::vector<Tile> distinct_tiles = tiles;
	//distinct_tiles.erase(unique(distinct_tiles.begin(), distinct_tiles.end()), distinct_tiles.end());
}