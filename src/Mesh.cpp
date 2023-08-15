#include "Mesh.h"

void Mesh::updateVertexHeights()
{
	for (int z = 0; z < this->meshLength; z++) {
		for (int x = 0; x < this->meshWidth; x++) {
			int vertexIndex = (z * this->meshWidth + x);
			this->vertexBuffer[(vertexIndex * 3) + 1] = this->heightMap[vertexIndex];
		}
	}

	/*glBindVertexArray(this->vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboID);*/
	//glBufferData(GL_ARRAY_BUFFER, this->vertexBuffer.size() * sizeof(float), this->vertexBuffer.data(), GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->vertexBuffer.size() * sizeof(float), this->vertexBuffer.data());
}

int Mesh::getTileSize()
{
	return this->tileSize;
}

int Mesh::getMeshWidth()
{
	return this->meshWidth;
}

int Mesh::getMeshLength()
{
	return this->meshLength;
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

	for (int z = 0; z < this->meshLength; z++) {
		for (int x = 0; x < this->meshWidth; x++) {
			this->vertexBuffer.push_back((x - (this->meshWidth / 2.0f)) * this->tileSize);
			this->vertexBuffer.push_back(0);
			this->vertexBuffer.push_back((z - (this->meshLength / 2.0f)) * this->tileSize);

			this->heightMap.push_back(0);
		}
	}

	for (int z = 0; z < this->meshLength - 1; z++) {
		for (int x = 0; x < this->meshWidth - 1; x++) {
			this->indexBuffer.push_back((z * (this->meshWidth)) + x); // 0
			this->indexBuffer.push_back((z * (this->meshWidth)) + (x + 1)); // 2
			this->indexBuffer.push_back(((z + 1) * (this->meshWidth)) + x);
			this->indexBuffer.push_back((z * (this->meshWidth)) + (x + 1));
			this->indexBuffer.push_back(((z + 1) * (this->meshWidth)) + (x + 1)); // 3
			this->indexBuffer.push_back(((z + 1) * (this->meshWidth)) + x); // 1
		}
	}

	glGenVertexArrays(1, &this->vaoID);

	glGenBuffers(1, &this->vboID);
	glGenBuffers(1, &this->iboID);

	glBindVertexArray(this->vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, this->vboID);
	glBufferData(GL_ARRAY_BUFFER, this->vertexBuffer.size() * sizeof(float), this->vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer.size() * sizeof(unsigned int), this->indexBuffer.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	this->shaderProgram = Shader("./src/shaders/Mesh.vert", "./src/shaders/Mesh.frag");
}

void Mesh::draw()
{
	this->shaderProgram.use();
	glBindVertexArray(this->vaoID);
	glDrawElements(GL_POINTS, this->indexBuffer.size(), GL_UNSIGNED_INT, 0);

}
