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

typedef std::tuple<std::vector<Tile>, SET_TYPE, STATE> SET;

std::ostream& operator << (std::ostream& os, const SET& set) {
	SUIT suit = std::get<0>(set)[0].get_suit();
	os << suit << " ";
	os << "[ ";
	for (const auto& tile : std::get<0>(set)) {
		os << tile.get_num_in_suit() << " ";
	}
	return os << "] ";
	if (std::get<2>(set) == STATE::CLOSE) os << "CL";
	else  os << "OP";
	return os;
}


std::ostream& operator << (std::ostream& os, const std::vector <SET>& sets) {
	for (const auto& set : sets) {
		os << set << ", ";
	}
	return os;
}
class Hand : private std::vector<Tile>{
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
					divide_suit_to_sets(suit, poss_sets); // BUG : picked from suit but from hand!
				}
			}
			poss_sets.clear();
		}
		poss_hands.erase(std::unique(poss_hands.begin(), poss_hands.end()), poss_hands.end());
		
		return;
	};

	~Hand() {
	};

	bool isValid() {
		if (poss_hands.size() == 0) return false;
		std::cout << *this << "\n";
		for (const auto& hand : poss_hands) {
			std::cout << hand << "\n";
		}
		return false;
	}

	friend std::ostream& operator << (std::ostream& os, const Hand& hand) {
		for (const auto& tile : hand) {
			os << tile << " ";
		}
		return os ;
	}
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

bool compareCounts(const std::tuple<Tile, int, int>& lhs, const std::tuple<Tile, int, int>& rhs) {
	return std::get<2>(lhs) > std::get<2>(rhs);
}


void Hand::calc_possible_chis(std::vector<Tile> suit, std::vector<SET>& sets) {
	if (suit.size() < 3) return;
	sets.clear();
	for (auto first = suit.begin(); first < suit.end()-2; first++) {
		for (auto second = first+1; second < suit.end()-1; second++) {
			for (auto third = second+1; third < suit.end(); third++) {
				int nums[3] = { first->get_num_in_suit(), second->get_num_in_suit(), third->get_num_in_suit() };
				STATE states[3] = { first->get_state(), second->get_state(), third->get_state() };
				if ((nums[1] == nums[0] + 1 && nums[2] == nums[0] + 2) && (states[0]==states[1] && states[0]==states[2])) {
					sets.push_back(SET(std::vector<Tile>{*first, * second, * third}, SET_TYPE::CHI, STATE::CLOSE));
				}
			}
		}
	}
	if (sets.size() > 1) {
		sets.erase(std::unique(sets.begin(), sets.end()), sets.end());
		std::vector<SET> distinct_sets = sets;
		sets.clear();
		for (const auto& set : distinct_sets) {
			sets.insert(sets.end(), 4, set);
		}
	}
}

void Hand::calc_possible_pars(std::vector<Tile> suit, std::vector<SET>& sets) {
	if (suit.size() < 2) return;
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) > 1) {
			sets.push_back(SET(std::vector<Tile>(2, std::get<0>(count)), SET_TYPE::PAIR, STATE::CLOSE));
		}
	}
}

void Hand::calc_possible_pons(std::vector<Tile> suit, std::vector<SET>& sets) {
	if (suit.size() < 3) return;
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) > 2) {
			sets.push_back(SET(std::vector<Tile>(3, std::get<0>(count)), SET_TYPE::PON, STATE::CLOSE));
		}
	}
}

void Hand::calc_possible_kans(std::vector<Tile> suit, std::vector<SET>& sets) {
	if (suit.size() < 4) return;
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

bool Hand::divide_suit_to_sets(std::vector<Tile> suit, std::vector<SET>& poss_sets) {
	if (suit.size() == 0) {
		std::sort(poss_sets.begin()+1, poss_sets.end());
		if (poss_sets.size() == 5)
			poss_hands.push_back(poss_sets);
		return true;
	}
	std::vector<Tile> suit_save = suit;
	std::vector<SET>  poss_sets_save = poss_sets;
	// suit do not contain a pair, only sets
	std::vector<SET> chis; calc_possible_chis(suit, chis);
	std::vector<SET> pons; calc_possible_pons(suit, pons);
	std::vector<SET> kans;
	if ((suit.size() - 2) % 3)
		calc_possible_kans(suit, kans);

	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	std::sort(tile_count.begin(), tile_count.end(), compareCounts);
	int count = std::get<2>(tile_count[0]);

	if (count > 2) {
		if (count == 4) {// 1 x kan || 1 x pon + 1 x chi || 4 x chi
			bool picked = false;
			for (const auto& pon : pons) {
				for (const auto& chi : chis) {
					suit = suit_save;
					poss_sets = poss_sets_save;
					if (pick_tiles(chi, suit) && pick_tiles(pon, suit)) {
						poss_sets.push_back(chi);
						poss_sets.push_back(pon);
						divide_suit_to_sets(suit, poss_sets);
					}
				}
			}
			if (chis.size() < 4) return false;
			suit      = suit_save;
			poss_sets = poss_sets_save;
			for (const auto& chi1 : chis) {
				for (const auto& chi2 : chis) {
					for (const auto& chi3 : chis) {
						for (const auto& chi4 : chis) {
							if (pick_tiles(chi1, suit) && pick_tiles(chi2, suit) && pick_tiles(chi3, suit) && pick_tiles(chi4, suit)) {
								poss_sets.push_back(chi1);
								poss_sets.push_back(chi2);
								poss_sets.push_back(chi3);
								poss_sets.push_back(chi4);
								divide_suit_to_sets(suit, poss_sets);
							}
							suit = suit_save;
							poss_sets = poss_sets_save;
						}
					}
				}
			}
		}
		if (count == 3) { // 1 x pon || 3 x chi
			if (pons.size() < 1) return false;
			for (const auto& pon : pons) {
				suit = suit_save;
				poss_sets = poss_sets_save;
				if (pick_tiles(pon, suit)) {
					poss_sets.push_back(pon);
					return divide_suit_to_sets(suit, poss_sets);
				}
				else return false;
			}

			if (chis.size() < 3) return false;
			for (const auto& chi1 : chis) {
				for (const auto& chi2 : chis) {
					for (const auto& chi3 : chis) {
						suit = suit_save;
						poss_sets = poss_sets_save;
						if (pick_tiles(chi1, suit) && pick_tiles(chi2, suit) && pick_tiles(chi3, suit)) {
							poss_sets.push_back(chi1);
							poss_sets.push_back(chi2);
							poss_sets.push_back(chi3);
							return divide_suit_to_sets(suit, poss_sets);
						}
					}
				}
			}
		}
	}else	if (count == 1) {
		for (const auto& chi : chis) { // 1 x chi
			suit = suit_save;
			poss_sets = poss_sets_save;
			if (!pick_tiles(chi, suit)) return false;
			poss_sets.push_back(chi);
			return divide_suit_to_sets(suit, poss_sets);
		}
	}else if (count == 2) { // 2 x chi
		if (chis.size() < 2) return false;
		for (const auto& chi1 : chis) {
			for (const auto& chi2 : chis) {
				suit = suit_save;
				poss_sets = poss_sets_save;
				if (pick_tiles(chi1, suit) && pick_tiles(chi2, suit)) {
					poss_sets.push_back(chi1);
					poss_sets.push_back(chi2);
					return divide_suit_to_sets(suit, poss_sets);
				}
			}
		}
	}

	return true;
};

bool operator == (const SET& lhs, const SET& rhs) {
	const std::vector<Tile>& ltiles = std::get<0>(lhs);
	const std::vector<Tile>& rtiles = std::get<0>(rhs);
	if (ltiles.size() != rtiles.size())
		return false;
	for (int i = 0; i < ltiles.size(); i++) {
		if (!(ltiles[i] == rtiles[i]))
			return false;
	}
	return true;
}
bool operator > (const SET& lhs, const SET& rhs) {
	const std::vector<Tile>& ltiles = std::get<0>(lhs);
	const std::vector<Tile>& rtiles = std::get<0>(rhs);
	if (ltiles.size() != rtiles.size())
		return false;
	for (int i = 0; i < ltiles.size(); i++) {
		return ltiles[i] > rtiles[i];
	}
	return false;
}
bool operator < (const SET& lhs, const SET& rhs) {
	const std::vector<Tile>& ltiles = std::get<0>(lhs);
	const std::vector<Tile>& rtiles = std::get<0>(rhs);
	if (ltiles.size() != rtiles.size())
		return false;
	for (int i = 0; i < ltiles.size(); i++) {
		return ltiles[i] < rtiles[i];
	}
	return false;
}