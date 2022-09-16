#pragma once
#include <iostream>
#include "tile.h"
#include <array>
#include <memory>
#include <algorithm>

enum class SET_TYPE{
	UNDEF,
	CHI,
	PON,
	KAN,
	PAIR
};

struct SET : private std::tuple<std::vector<Tile>, SET_TYPE, STATE> {
	SET(std::vector<Tile>, SET_TYPE, STATE) : std::tuple<std::vector<Tile>, SET_TYPE, STATE>(){

	};
	SET() {
		std::get<1>(*this) = SET_TYPE::UNDEF;
		std::get<2>(*this) = STATE::CLOSE;
	}
	const std::vector<Tile>& get_tiles() const {
		return std::get<0>(*this);
	}
	SET_TYPE  get_type() const {
		return std::get<1>(*this);
	}
	STATE  get_state() const {
		return std::get<2>(*this);
	}
	SUIT  get_suit() const {
		return get_tiles()[0].get_suit();
	}
};

std::ostream& operator << (std::ostream& os, const SET& set) {
	SUIT suit = set.get_suit();
	os << suit << " ";
	os << "[ ";
	for (const auto& tile : set.get_tiles()) {
		os << tile.get_num_in_suit() << " ";
	}
	return os << "] ";
	if (set.get_state() == STATE::CLOSE) os << "CL";
	else  os << "OP";
	return os;
}


std::ostream& operator << (std::ostream& os, const std::vector <SET>& sets) {
	for (const auto& set : sets) {
		os << set << ", ";
	}
	return os;
}

struct PermutationEngine {
public:
	void calc(size_t offset, size_t set, size_t k) {
		if (k == 0) {
			if (permutation.size() > 0)
			permutations.push_back(permutation);
			return;
		}
		for (size_t i = offset; i <= set - k; ++i) {
			permutation.push_back(i);
			calc(i + 1, set, k - 1);
			permutation.pop_back();
		}
	}
	const std::vector<std::vector<size_t>> getPermutations() {
		return permutations;
	}

	template<typename T>
	static std::vector<std::vector<T>> getPermutations(std::vector<T> set, size_t k) {
		PermutationEngine permut_engine;
		permut_engine.calc(0, set.size(), k);
		std::vector<std::vector<size_t>> number_permuts = permut_engine.getPermutations();
		std::vector<std::vector<T>> permuts(number_permuts.size());
		for (int perm = 0; perm < number_permuts.size(); perm++)
			for (auto num : number_permuts[perm])
				permuts[perm].push_back(set[num]);
		return permuts;
	}
private:
	std::vector<size_t> permutation;
	std::vector<std::vector<size_t>> permutations;
};


class HandSeeker : private std::vector<Tile>{
public:
	HandSeeker(const std::vector<int>& hand_tiles, const std::vector<int>& dora_poiters, int win_tile) {
		for (size_t i = 0; i < dora_poiters.size(); i++) {
			dora_tiles.push_back(Tile::get_tile_by_index(Tile::get_next_tile_index(dora_poiters[i])));
		}
		for (int i = 0; i < hand_tiles.size(); i++) {
			Tile new_tile = Tile::get_tile_by_index(hand_tiles[i]);
			new_tile.set_position(i);
			check_dora_tile(new_tile);
			this->push_back(new_tile);
		}
	}

	~HandSeeker() {
	};

	bool isValid() {
		if (poss_hands.size() == 0) return false;
		std::cout << *this << "\n";
		for (const auto& hand : poss_hands) {
			std::cout << hand << "\n";
		}
		return true;
	}
	void find_possible_hands();

	friend std::ostream& operator << (std::ostream& os, const HandSeeker& hand) {
		for (const auto& tile : hand) {
			os << tile << " ";
		}
		return os ;
	}
private:
	std::vector<std::vector<Tile>> suits;
	std::vector <SET> sets;
	std::vector < std::vector<SET>> poss_hands;
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
};

void HandSeeker::find_possible_hands() {
	if (this->size() < 14)
		return;
	sort(this->begin(), this->end(), Tile::compareTiles);
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(*this);
	for (const auto& count : tile_count)
		if (std::get<2>(count) > 4)
			return;

	std::vector<SET> pars; calc_possible_pars(*this, pars);
	std::vector<SET> kans; calc_possible_kans(*this, kans);

	size_t num_kans_in_hand = this->size() - 14;
	std::vector <std::pair<SET, std::vector<SET>>> pair_and_kans; // any combination of pair and kans


	std::vector <std::vector<SET>> kans_permut_sets = PermutationEngine::getPermutations<SET>(kans, num_kans_in_hand);
	for (const auto& par : pars) {
		if (kans_permut_sets.size() > 0) {
			for (const auto& kan_set : kans_permut_sets) {
				pair_and_kans.push_back(std::make_pair(par, kan_set));
			}
		}
		else
			pair_and_kans.push_back(std::make_pair(par, std::vector<SET>(0)));
	}

	for (const auto& par_kan : pair_and_kans) {
		std::vector<Tile> hand = *this;
		std::vector <SET> poss_sets;
		if (pick_tiles(par_kan.first, hand)) {
			bool OK = true;
			std::vector<Tile> hand_without_pair = hand;
			for (const auto& kan : par_kan.second) {
				if (!pick_tiles(kan, hand)) OK = false;
			}
			if (OK) {
				poss_sets.push_back(par_kan.first);
				for (const auto& kan : par_kan.second) {
					poss_sets.push_back(kan);
				}
				std::vector<Tile> hand_without_pair_and_kans = hand;
				suits.clear();
				divide_hand_to_suits(hand);
				std::vector <SET> suit_sets;
				for (auto suit : suits) {
					if (divide_suit_to_sets(suit, poss_sets)) {
						for (auto set = poss_sets.begin() + 1; set < poss_sets.end(); ++set) {
							if (!pick_tiles(*set, hand))
								break;
						}
					}
					else break;
				}
				hand = hand_without_pair_and_kans;
			}
			else
				hand = hand_without_pair;
		}
		poss_sets.clear();
	}
	poss_hands.erase(std::unique(poss_hands.begin(), poss_hands.end()), poss_hands.end());

	return;
};


void HandSeeker::divide_hand_to_suits(std::vector<Tile> tiles) {
	std::vector<Tile> suit_set;
	auto it = tiles.begin();
	SUIT suit = it->get_suit();

	std::vector<SUIT> dist_suits;
	for (const auto& tile : tiles)
		dist_suits.push_back(tile.get_suit());
	dist_suits.erase(unique(dist_suits.begin(), dist_suits.end()), dist_suits.end());
	suits.resize(dist_suits.size());

	int suit_num = 0;
	while (suit_num < dist_suits.size() && it < tiles.end()) {
		if (it->get_suit() == dist_suits[suit_num]) {
			suits[suit_num].push_back(*it);
			it++;
		}
		else
			suit_num++;
	}
	return;
};

std::vector<std::tuple<Tile, int, int>>  HandSeeker::calc_count(std::vector<Tile> tiles) {
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

void HandSeeker::calc_possible_chis(std::vector<Tile> suit, std::vector<SET>& sets) {
	SUIT suit_curr = suit.begin()->get_suit();
	if (suit_curr == SUIT::DRAGON || suit_curr == SUIT::WIND || suit_curr == SUIT::UNDEF)
		return;

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

void HandSeeker::calc_possible_pars(std::vector<Tile> suit, std::vector<SET>& sets) {
	if (suit.size() < 2) return;
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) > 1) {
			sets.push_back(SET(std::vector<Tile>(2, std::get<0>(count)), SET_TYPE::PAIR, STATE::CLOSE));
		}
	}
}

void HandSeeker::calc_possible_pons(std::vector<Tile> suit, std::vector<SET>& sets) {
	if (suit.size() < 3) return;
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) > 2) {
			sets.push_back(SET(std::vector<Tile>(3, std::get<0>(count)), SET_TYPE::PON, STATE::CLOSE));
		}
	}
}

void HandSeeker::calc_possible_kans(std::vector<Tile> suit, std::vector<SET>& sets) {
	if (suit.size() < 4) return;
	sets.clear();
	std::vector<std::tuple<Tile, int, int>> tile_count = calc_count(suit);
	for (const auto& count : tile_count) {
		if (std::get<2>(count) == 4) {
			sets.push_back(SET(std::vector<Tile>(4, std::get<0>(count)), SET_TYPE::KAN, STATE::CLOSE));
		}
	}
}

bool HandSeeker::pick_tiles(SET set, std::vector<Tile>& suit) {
	const std::vector<Tile>& tiles = set.get_tiles();
	if (suit.size() < tiles.size())
		return false;

	for (const auto& tile : tiles) {
		auto it = std::find(suit.begin(), suit.end(), tile);
		if (it != suit.end()) {
			suit.erase(it);
		} else
			return false;
	}
	return true;
}

bool HandSeeker::divide_suit_to_sets(std::vector<Tile> suit, std::vector<SET>& poss_sets) {
	if (suit.size() == 0) {
		std::sort(poss_sets.begin()+1, poss_sets.end());
		if (poss_sets.size() == 5)
			poss_hands.push_back(poss_sets);
		return true;
	}	else if (suit.size() < 3)
		return false;

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
			for (std::vector<SET>::iterator chi1 = chis.begin(); chi1 < chis.end(); chi1++) {
				for (std::vector<SET>::iterator chi2 = chi1; chi2 < chis.end(); chi2++) {
					for (std::vector<SET>::iterator chi3 = chi2; chi3 < chis.end(); chi3++) {
						for (std::vector<SET>::iterator chi4 = chi3; chi4 < chis.end(); chi4++) {
							if (pick_tiles(*chi1, suit) && pick_tiles(*chi2, suit) && pick_tiles(*chi3, suit) && pick_tiles(*chi4, suit)) {
								poss_sets.push_back(*chi1);
								poss_sets.push_back(*chi2);
								poss_sets.push_back(*chi3);
								poss_sets.push_back(*chi4);
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
					divide_suit_to_sets(suit, poss_sets);
				}
				else return false;
			}

			if (chis.size() < 3) return false;
			for (std::vector<SET>::iterator chi1 = chis.begin(); chi1 < chis.end(); chi1++) {
				for (std::vector<SET>::iterator chi2 = chi1; chi2 < chis.end(); chi2++) {
					for (std::vector<SET>::iterator chi3 = chi2; chi3 < chis.end(); chi3++) {
						suit = suit_save;
						poss_sets = poss_sets_save;
						if (pick_tiles(*chi1, suit) && pick_tiles(*chi2, suit) && pick_tiles(*chi3, suit)) {
							poss_sets.push_back(*chi1);
							poss_sets.push_back(*chi2);
							poss_sets.push_back(*chi3);
							divide_suit_to_sets(suit, poss_sets);
						}
					}
				}
			}
			return false;
		}
	}else	if (count == 1) {
		for (const auto& chi : chis) { // 1 x chi
			suit = suit_save;
			poss_sets = poss_sets_save;
			if (!pick_tiles(chi, suit)) return false;
			poss_sets.push_back(chi);
			divide_suit_to_sets(suit, poss_sets);
		}
		return false;
	}else if (count == 2) { // 2 x chi
		if (chis.size() < 2) return false;
		for (const auto& chi1 : chis) {
			for (const auto& chi2 : chis) {
				suit = suit_save;
				poss_sets = poss_sets_save;
				if (pick_tiles(chi1, suit) && pick_tiles(chi2, suit)) {
					poss_sets.push_back(chi1);
					poss_sets.push_back(chi2);
					divide_suit_to_sets(suit, poss_sets);
				}
			}
		}
	}
	return false;
};

bool operator == (const SET& lhs, const SET& rhs) {
	const std::vector<Tile>& ltiles = lhs.get_tiles();
	const std::vector<Tile>& rtiles = rhs.get_tiles();
	if (ltiles.size() != rtiles.size())
		return false;
	for (int i = 0; i < ltiles.size(); i++) {
		if (!(ltiles[i] == rtiles[i]))
			return false;
	}
	return true;
}
bool operator > (const SET& lhs, const SET& rhs) {
	const std::vector<Tile>& ltiles = lhs.get_tiles();
	const std::vector<Tile>& rtiles = rhs.get_tiles();
	if (ltiles.size() != rtiles.size())
		return false;
	for (int i = 0; i < ltiles.size(); i++) {
		if (ltiles[i] > rtiles[i])
			return true;
		if (ltiles[i] < rtiles[i])
			return false;
	}
	return false;
}
bool operator < (const SET& lhs, const SET& rhs) {
	const std::vector<Tile>& ltiles = lhs.get_tiles();
	const std::vector<Tile>& rtiles = rhs.get_tiles();
	if (ltiles.size() != rtiles.size())
		return false;
	for (int i = 0; i < ltiles.size(); i++) {
		if (ltiles[i] < rtiles[i])
			return true;
		if (ltiles[i] > rtiles[i])
			return false;
	}
	return false;
}