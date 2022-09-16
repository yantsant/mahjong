#pragma once
#include "tile.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>

struct Rect { // row data from pytorch
	Rect(float height, float width, float centerX, float centerY) {
		h = height;
		w = width;
		c = std::make_pair(centerX, centerY);
	};
	std::pair<float, float> getCenter() const {
		return c;
	}
	float getWidth() const {
		return w;
	}
	float getHeight() const {
		return h;
	}
private: 
	float h, w; // height/width
	std::pair<float, float> c; // center
};

class PrepareData {
public:
	PrepareData() {
		readDataFile("..//data//data3.txt");
		std::vector <int> result = getTileHandIndex();
	}
	void readDataFile(std::string path) {
		std::ifstream data;
		data.open(path);
		std::string line;
		int lines = 0;
		while (getline(data, line))		{
			lines++;
		}

		data.close();
		data.open(path);
		int idx;
		float h, w, cx, cy;
		for (size_t i = 0; i < lines; i++)		{
			data >> idx >> w >> h >> cx >> cy ;
			Tile tile = Tile::get_tile_by_index(idx);
			if (h < w)
				tile.set_state(STATE::OPEN);
			tileHand.push_back(std::make_pair(tile, Rect(h, w, cx, cy)));
		}

		data.close();
	}
	std::vector <int> getTileHandIndex() {
		std::vector <int> result;
		std::pair <float, float> avgCenter(0, 0);
		float avgHeight = 0, avgWidth = 0;
		for (const auto& pos : tileHand) {
			avgCenter.first += pos.second.getCenter().first;
			avgCenter.second += pos.second.getCenter().second;
			avgHeight += pos.second.getHeight();
			avgWidth  += pos.second.getWidth();
		}
		avgHeight /= tileHand.size();
		avgWidth  /= tileHand.size();
		avgCenter.first /= tileHand.size();
		avgCenter.second /= tileHand.size();
		for (auto& pos : tileHand) {
			if (pos.second.getCenter().second > avgCenter.second + 0.5 * avgHeight) {
				winTile = pos.first;
			}
		}
		return result;
	}
private:
	Tile winTile;
	std::vector<std::pair<Tile, Rect>> tileHand;
};