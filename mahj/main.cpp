#include "set.h"

int main() {
	std::vector<TILE> hand;
	for (int i = 0; i < 37; i++)
	{
		hand.push_back(get_tile_by_index(i));
	}
	return 0;
}