#include "cost.h"
#include <random>
std::vector<int> valid_hand_kan() {
	return std::vector<int> {0,0,0,0, 1,1,1,1, 2,2,2, 2,3,4, 35,35};
}
std::vector<int> valid_hand() {
	return std::vector<int>{0,1,2, 0,1,2, 11,12,13, 0,1,2, 35,35};
}

std::vector<int> invalid_hand() {
	return std::vector<int> {0,1,2, 4,5,6, 8,8, 16, 24, 27, 28, 35, 36};
}
std::vector<int> ryanpeiko_hand() {
	return std::vector<int> {0,0, 0,0,1,1,2,2, 1,1,2,2,3,3};
}
int main() {
	std::vector<int> tile = ryanpeiko_hand();
	std::vector<int> dora_pointer;

	//for (int i = 0; i < 3; i++) {
	//	dora_pointer.push_back(13);
	//}
	HandSeeker hand(tile, dora_pointer, 3);
	hand.find_possible_hands();
	hand.isValid();


	srand(time(NULL));
	size_t valid = 0;
	float possib_tile[37] = {0.0f};
	for (size_t j = 0; j < 10000000; j++)
	{
		for (int i = 0; i < 14; i++) {
			tile[i] = rand() % 37;
			//possib_tile[tile[i]]++;
		}
		HandSeeker hand(tile, dora_pointer, 3);
		hand.find_possible_hands();
		if (hand.isValid()) {
			valid++;
			std::cout << double(valid*100) / j <<"% ";
		}
		//if (j % 1000 == 0) {
		//	for (int i = 0; i < 37; i++)
		//		std::cout << 100 * possib_tile[i] / (14*j) << "% ";
		//	std::cout << "\n";
		//}
	}
	return 0;
}