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

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &this->vaoID);
	glDeleteBuffers(1, &this->vboID);
	glDeleteBuffers(1, &this->iboID);
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
			this->vertexBuffer.push_back((x - (this->meshWidth /(float) 2)) * this->tileSize);
			this->vertexBuffer.push_back((y - (this->meshLength /(float) 2)) * this->tileSize);
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

	glGenVertexArrays(1, &this->vaoID);

	glGenBuffers(1, &this->vboID);
	glGenBuffers(1, &this->iboID);

	glBindVertexArray(this->vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, this->vboID);
	glBufferData(GL_ARRAY_BUFFER, this->vertexBuffer.size(), this->vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer.size(), this->indexBuffer.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	this->shaderProgram = Shader("./src/shaders/Mesh.vert", "./src/shaders/Mesh.frag");
}

void Mesh::draw()
{
	this->shaderProgram.use();
	glBindVertexArray(this->vaoID);
	glDrawElements(GL_TRIANGLES, this->indexBuffer.size(), GL_UNSIGNED_INT, 0);

}
