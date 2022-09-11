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

		std::vector<SET> pars; calc_possible_pars(*this, pars);
		for (const auto& par : pars) {
			std::vector<Tile> hand = *this;
			std::vector <SET> poss_sets;
			if (pick_tiles(par, hand)) {
				poss_sets.push_back(par);
				suits.clear();
				divide_hand_to_suits(hand);
				std::vector <SET> suit_sets;
				for (auto suit : suits) {
					if (divide_suit_to_sets(suit, suit_sets)) {
						poss_sets.insert(poss_sets.end(), suit_sets.begin(), suit_sets.end());
					}
				}
			}
			if (poss_sets.size() == 5) {
				poss_hands.push_back(poss_sets);
			}
			poss_sets.clear();
		}
		return;
	};

	~Hand() {
	};

private:
	std::vector<std::vector<Tile>> suits;
	std::vector <SET> sets;
	std::vector <std::vector <SET>> poss_hands;
	std::vector<Tile> dora_tiles;
	void check_dora_tile(Tile& tile) {
		for (const auto& dora : dora_tiles) {
			if (tile == dora)
				tile.set_as_dora(DORA::DORA);
		}
	};
	void calc_possible_pars(std::vector<Tile> suit, std::vector<SET>& sets);
	void calc_possible_chis(std::vector<Tile> suit, std::vector<SET>& sets);
	void calc_possible_pons(std::vector<Tile> suit, std::vector<SET>& sets);
	void calc_possible_kans(std::vector<Tile> suit, std::vector<SET>& sets);

	bool pick_tiles(SET set, std::vector<Tile>& suit);

	std::vector<std::tuple<Tile, int, int>> calc_count(std::vector<Tile> tiles);
	void divide_hand_to_suits(std::vector<Tile> tiles);
	bool divide_suit_to_sets(std::vector<Tile> suit, std::vector<SET>& sets);
	bool _divide_suit_to_sets(std::vector<Tile> suit);
	bool find_par(std::vector<Tile>& tiles, SET& new_set);
	bool find_chi(std::vector<Tile>& tiles, SET& new_set);
	bool find_pon(std::vector<Tile>& tiles, SET& new_set);
	bool make_set(std::vector<Tile> tiles, SET& new_set);
	bool make_par(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
	bool make_pon(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
	bool make_chi(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
	bool make_kan(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set);
};

void Hand::divide_hand_to_suits(std::vector<Tile> tiles) {
	std::vector<Tile> suit_set;
	auto it = tiles.begin();
	SUIT suit = it->get_suit();

	while (tiles.size() > 0) {
		if (it->get_suit() == suit) {
			suit_set.push_back(*it);
			tiles.erase(tiles.begin());
			it = tiles.begin();
		} else  {
			suit = it->get_suit();
			suits.push_back(suit_set);
			if (suit_set.size() == tiles.size()) {
				tiles.clear();
				break;
			}
			suit_set.clear();
		};
	};
	suits.push_back(suit_set);
};


bool Hand::make_par(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set) {
	auto it = first;
	if (!(*it == *(it + 1))) return false;
	std::get<1>(new_set) = SET_TYPE::PAIR;
	std::get<2>(new_set) = STATE::CLOSE;
	do {
		std::get<0>(new_set).push_back(*it);
		it++;
	} while (!(it >= last));
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
	} while (!(it >= last));
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
	return true;
}

bool Hand::make_chi(const std::vector<Tile>::iterator& first, const std::vector<Tile>::iterator& last, SET& new_set) {
	auto it = first;
	int nums[3] = { it->get_num_in_suit(), (it+1)->get_num_in_suit(), (it+2)->get_num_in_suit()};
	if (!(nums[1] == nums[0]+1 && nums[2] == nums[0]+2)) return false;
	std::get<1>(new_set) = SET_TYPE::CHI;
	std::get<2>(new_set) = STATE::CLOSE;
	do {
		std::get<0>(new_set).push_back(*it);
		if (it->get_state() == STATE::OPEN)
			std::get<2>(new_set) = STATE::OPEN;
		it++;
	} while (!(it >= last));
	return true;
}



std::vector<std::tuple<Tile, int, int>>  Hand::calc_count(std::vector<Tile> tiles) {
	std::vector<std::tuple<Tile, int, int>> res;
	std::vector<Tile>  dist_tiles = tiles;
	dist_tiles.erase(unique(dist_tiles.begin(), dist_tiles.end()), dist_tiles.end());
	auto it = tiles.begin();
	for (const auto& tile : dist_tiles)	{
		std::tuple<Tile, int, int> cnt = { tile, tile.get_num_in_suit(), 0 };
		while (tile == *it) {
			std::get<2>(cnt)++;
			++it;
			if (it == tiles.end()) 
				break;
		}
		res.push_back(cnt);
	}
	return res;
}

bool Hand::find_par(std::vector<Tile>& tiles, SET& new_set) {
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(tiles);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) == 2) {
			auto first = std::find(tiles.begin(), tiles.end(), std::get<0>(count));
			make_par(first, first + 2, new_set);
			tiles.erase(first, first + 2);
			return true;
		}
	};
	return false;
}

bool Hand::find_pon(std::vector<Tile>& tiles, SET& new_set) {
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(tiles);

	for (const auto& count : tile_count) {
		if (std::get<2>(count) > 2) {
			auto first = std::find(tiles.begin(), tiles.end(), std::get<0>(count));
			make_pon(first, first + 3, new_set);
			tiles.erase(first, first + 3);
			return true;
		}
	}
	return false;
}
bool Hand::make_set(std::vector<Tile> tiles, SET& new_set) {
	if (tiles.size() != 3)
		return false;

	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(tiles);
	bool res = false;
	if (tile_count.size() == 1)
		res = make_pon(tiles.begin(), tiles.end(), new_set);
	else
		res = make_chi(tiles.begin(), tiles.end(), new_set);
	return res;
}

bool Hand::find_chi(std::vector<Tile>& suit, SET& new_set) {
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	std::vector<Tile> chi;
	STATE state = STATE::CLOSE;
	auto tile_it = tile_count.begin();

	for (const auto& count : tile_count) {
		if (std::get<2>(count) == 1) {
			Tile tile = std::get<0>(count);
			//auto first = 
		}
	}

	while (suit.size() > 0 && tile_it < tile_count.end()) {
		Tile tile = std::get<0>(*tile_it);
		if (std::get<2>(*tile_it) == 1) { // must be included in chi
			if (chi.size() > 0) {
				if (tile.get_num_in_suit() == chi.back().get_num_in_suit() + 1) {
					chi.push_back(tile);
					if (tile.get_state() == STATE::CLOSE) state = STATE::CLOSE;
				}
			}
			else {
				state = tile.get_state();
				chi.push_back(tile);
			}
			if (chi.size() == 3) {
				sets.push_back(SET(chi, SET_TYPE::CHI, state));
				for (const auto& chi_tile : chi) {
					auto it = std::find(suit.begin(), suit.end(), chi_tile);
					suit.erase(it);
				}
				chi.clear();
			}
		}
		++tile_it;
	}
	return false;
}

bool compareCounts(const std::tuple<Tile, int, int>& lhs, const std::tuple<Tile, int, int>& rhs) {
	return std::get<2>(lhs) > std::get<2>(rhs);
}

void Hand::calc_possible_chis(std::vector<Tile> suit, std::vector<SET>& sets) {
	sets.clear();
	for (auto first = suit.begin(); first < suit.end()-2; first++) {
		for (auto second = first+1; second < suit.end()-1; second++) {
			for (auto third = second+1; third < suit.end(); third++) {
				int nums[3] = { first->get_num_in_suit(), second->get_num_in_suit(), third->get_num_in_suit() };
				if ((nums[1] == nums[0] + 1 && nums[2] == nums[0] + 2)) {
					sets.push_back(SET(std::vector<Tile>{*first, * second, * third}, SET_TYPE::CHI, STATE::CLOSE));
				}
			}
		}
	}
}

void Hand::calc_possible_pars(std::vector<Tile> suit, std::vector<SET>& sets) {
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) > 1) {
			sets.push_back(SET(std::vector<Tile>(2, std::get<0>(count)), SET_TYPE::PAIR, STATE::CLOSE));
		}
	}
}

void Hand::calc_possible_pons(std::vector<Tile> suit, std::vector<SET>& sets) {
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) > 2) {
			sets.push_back(SET(std::vector<Tile>(3, std::get<0>(count)), SET_TYPE::PON, STATE::CLOSE));
		}
	}
}

void Hand::calc_possible_kans(std::vector<Tile> suit, std::vector<SET>& sets) {
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) == 4) {
			sets.push_back(SET(std::vector<Tile>(4, std::get<0>(count)), SET_TYPE::KAN, STATE::CLOSE));
		}
	}
}

bool Hand::pick_tiles(SET set, std::vector<Tile>& suit) {
	if (suit.size() < std::get<0>(set).size())
		return false;

	for (const auto& tile : std::get<0>(set)) {
		auto it = std::find(suit.begin(), suit.end(), tile);
		if (it != suit.end()) {
			suit.erase(it);
		} else
			return false;
	}
	return true;
}

bool Hand::divide_suit_to_sets(std::vector<Tile> suit, std::vector<SET> &poss_sets) {
	if (suit.size() == 0) 
		return true;
	// suit do not contain a pair, only sets
	std::vector<SET> chis; calc_possible_chis(suit, chis);
	std::vector<SET> pons; calc_possible_pons(suit, pons);
	std::vector<SET> kans; 
	if ((suit.size() - 2) % 3)
		calc_possible_kans(suit, kans);

	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	std::sort(tile_count.begin(), tile_count.end(), compareCounts);

	if (std::get<2>(tile_count[0]) == 4) { // 1 x kan || 1 x pon + 1 x chi || {1 x par} + 2 x chi
		// 1 x kan || 1 x pon + 1 x chi
		bool picked = false;
		for (const auto& pon : pons) {
			for (const auto& chi : chis) {
				if (pick_tiles(chi, suit) && pick_tiles(pon, suit)) {
					poss_sets.push_back(chi);
					poss_sets.push_back(pon);
					return divide_suit_to_sets(suit, poss_sets);
				}
			}
		}
	} else if (std::get<2>(tile_count[0]) == 3) { // 1 x pon || 3 x chi
		if (pons.size() < 1) return false;
		for (const auto& pon : pons) {
			if (pick_tiles(pon, suit)) {
				poss_sets.push_back(pon);
				return divide_suit_to_sets(suit, poss_sets);
			} else return false;
		}

		if (chis.size() < 3) return false;
		for (const auto& chi1 : chis) {
			for (const auto& chi2 : chis) {
				for (const auto& chi3 : chis) {
					if (pick_tiles(chi1, suit) && pick_tiles(chi2, suit) && pick_tiles(chi3, suit)) {
						poss_sets.push_back(chi1);
						poss_sets.push_back(chi2);
						poss_sets.push_back(chi3);
						return divide_suit_to_sets(suit, poss_sets);
					}
				}
			}
		}
	} else	if (std::get<2>(tile_count[0]) == 1) {
		for (const auto& chi : chis) { // 1 x chi
			if (!pick_tiles(chi, suit)) return false;
			poss_sets.push_back(chi);
			return divide_suit_to_sets(suit, poss_sets);
		}
	}	else if (std::get<2>(tile_count[0]) == 2) { // {1 x par} || 2 x chi
		if (chis.size() < 2) return false;
		else if (chis.size() == 2) { // 2 x chi
			for (const auto& chi : chis) {
				if (!pick_tiles(chi, suit)) return false;
				poss_sets.push_back(chi);
			}
			return divide_suit_to_sets(suit, poss_sets);
		} else {
			for (const auto& chi1 : chis) {
				for (const auto& chi2 : chis) {
					if (pick_tiles(chi1, suit) && pick_tiles(chi2, suit)) {
						poss_sets.push_back(chi1);
						poss_sets.push_back(chi2);
						return divide_suit_to_sets(suit, poss_sets);
					}
				}
			}
		}
		
	}	

	return true;
}

bool Hand::_divide_suit_to_sets(std::vector<Tile> suit) {
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);

	if (suit.size() == 1) {
		return false;
	} else	if (suit.size() == 2) { // only a pair
		SET set;
		if (find_par(suit, set)) {
			sets.push_back(set);
		} else return false;
	}
	else if (suit.size() == 3) { // may be chi or pon
		SET set;
		if (make_set(suit, set)) {
			sets.push_back(set);
			suit.clear();
		} else return false;
	}	else if (suit.size() == 4) {
		SET set;
		if (tile_count.size() == 1) {
			make_kan(suit.begin(), suit.end(), set);
			sets.push_back(set);
		} else if (tile_count.size() == 2) {
			auto first = suit.begin();
			if (make_par(first, first+2, set))
				sets.push_back(set);
			else return false;
			if (make_par(first+2, first+4, set))
				sets.push_back(set);
			else return false;
		}
		suit.clear();
	}else if (suit.size() == 5) { // pair and set
		SET set1;
		if (find_par(suit, set1)) {
			sets.push_back(set1);
		}  else return false;
		SET set2;
		if (make_set(suit, set2)) {
			sets.push_back(set2);
			suit.clear();
		} else return false;
	}

	if (suit.size() == 0)
		return true;

	tile_count = calc_count(suit);

	if (suit.size() ==  6) { // two sets | tree pair
		if (tile_count.size() == 2) {
			if (std::get<2>(tile_count[0]) == std::get<2>(tile_count[1])) { // two pons
				SET set;
				if (make_pon(suit.begin(), suit.begin() + 3, set)) {
					sets.push_back(set);
				} else return false;
				if (make_pon(suit.begin() + 3, suit.begin() + 6, set)) {
					sets.push_back(set);
				} else return false;
			} else{ // pair and kan 
				SET set;
				if (find_par(suit, set)) {
					sets.push_back(set);
				}else return false;
				if (make_kan(suit.begin(), suit.end(), set)) {
					sets.push_back(set);
				} else return false;
			}
		} else if (tile_count.size() == 3 || tile_count.size() == 4) { // intersected chi & pon || distinct chi & pon || two intersected chi (two tiles)
			SET set;
			if (find_pon(suit, set)) {
				sets.push_back(set);
			}  else return false;
			if (make_chi(suit.begin(), suit.end(), set)) {
				sets.push_back(set);
			}  else return false;
		} else if (tile_count.size() == 5) { // two intersected chi (one tile)
		} else if (tile_count.size() == 6) { // two distinct chi
		}
		suit.clear();
	}	else if (suit.size() ==  7) { // set & kan 
	}	else if (suit.size() ==  8) { // two sets & pair | two kans
	}	else if (suit.size() ==  9) { // three sets
	}	else if (suit.size() == 10) { // two sets & pair | two kans
	}	else if (suit.size() == 11) { // two sets & pair | two kans
	}	else if (suit.size() == 12) { // four sets
	}	else if (suit.size() == 13) { // two sets & pair | two kans
	}	else if (suit.size() == 14) { // four sets & pair | seven pairs
	}
	return true;
	//tiles = suit;
	//sort(tiles.begin(), tiles.end(), Tile::compareTiles);
	//std::vector<Tile> distinct_tiles = tiles;
	//distinct_tiles.erase(unique(distinct_tiles.begin(), distinct_tiles.end()), distinct_tiles.end());
}