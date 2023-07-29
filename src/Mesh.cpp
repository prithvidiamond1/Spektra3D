#include "Mesh.h"

void Mesh::updateVertexHeights()
{
	for (int y = 0; y < this->meshLength; y++) {
		for (int x = 0; x < this->meshWidth; x++) {
			int vertexIndex = (y * this->meshWidth + x);
			this->vertexBuffer[(vertexIndex * 3) + 2] = this->heightMap[vertexIndex];
		}
	}
}

Mesh::Mesh(int meshSize, int tileSize): Mesh(meshSize, meshSize, tileSize)
{

}

Mesh::Mesh(int meshWidth, int meshLength, int tileSize)
{
	this->meshWidth = meshWidth;
	this->meshLength = meshLength;
	this->tileSize = tileSize;

	for (int y = 0; y < this->meshLength; y++) {
		for (int x = 0; x < this->meshWidth; x++) {
			this->vertexBuffer.push_back((x - (this->meshWidth / 2)) * this->tileSize);
			this->vertexBuffer.push_back((y - (this->meshWidth / 2)) * this->tileSize);
			this->vertexBuffer.push_back(0);

			this->heightMap.push_back(0);
		}
	}

	for (int y = 0; y < this->meshLength - 1; y++) {
		for (int x = 0; x < this->meshWidth - 1; x++) {
			this->indexBuffer.push_back((y * (this->meshWidth)) + x);
			this->indexBuffer.push_back((y * (this->meshWidth)) + (x + 1));
			this->indexBuffer.push_back(((y + 1) * (this->meshWidth)) + x);
			this->indexBuffer.push_back((y * (this->meshWidth)) + (x + 1));
			this->indexBuffer.push_back(((y + 1) * (this->meshWidth)) + (x + 1));
			this->indexBuffer.push_back(((y + 1) * (this->meshWidth)) + x);
		}
	}
}
