#include "hand.h"
#include <random>
int main() {
	std::vector<int> tile;
	std::vector<int> dora_pointer;
	for (int i = 0; i < 14; i++)	{
		tile.push_back(rand()%37);
	}
	for (int i = 0; i < 3; i++) {
		dora_pointer.push_back(13);
	}
	Hand hand(tile, dora_pointer, 3);
	return 0;
}