#include "hand.h"

int main() {
	std::vector<int> tile;
	std::vector<int> dora_pointer;
	for (int i = 0; i < 37; i++)	{
		tile.push_back(i);
	}
	for (int i = 0; i < 3; i++) {
		dora_pointer.push_back(13);
	}
	Hand hand(tile, dora_pointer, 3);
	return 0;
}