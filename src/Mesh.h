#pragma once

#include <vector>

class Mesh {
private:
	int tileSize;
	int meshWidth;
	int meshLength;

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

public:
	std::vector<float> heightMap;	// too important to not access and modify but also very large to keep copying and destroying copies of

	Mesh(int meshSize, int tileSize);

	Mesh(int meshWidth, int meshLength, int tileSize);

	void draw();

	void updateVertexHeights();

};