#include "Water.h"


Water::Water(GLfloat height) : numTiles(NUMBER_OF_TILES_ACROSS), tileSize(TEXTURE_SIZE), height(height)
{
	std::cout << "Generating Water" << std::endl;

	//build VBO's
	buildVertexVBO();
	buildNormalsVBO();
	buildUVVBO();
	//build EBO	
	buildIndexEBO();
	//build VAO
	buildVAO();
	buildTexture();
}


void Water::draw()
{
	glBindTexture(GL_TEXTURE_2D, waterTexture);
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indicies.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Water::~Water() {}

void Water::buildVertexVBO() {
	GLfloat half = (GLfloat)this->numTiles / 2.0f;
	//Creates a plane at y=height, with given width and length, centered at the origin
	for (int l = 0; l <= this->numTiles; l++) {
		for (int w = 0; w <= this->numTiles; w++) {
			this->vertices.push_back(glm::vec3((GLfloat)(w * this->tileSize - half * this->tileSize), this->height, (GLfloat)(l * this->tileSize - half * this->tileSize)));
		}
	}

	glGenBuffers(1, &this->vertex_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Water::buildNormalsVBO()
{	
	//Water is a flat plane in x and z, all normals are in the positive y direction
	for (int i = 0; i < vertices.size(); i++) {
		normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Water::buildUVVBO() {

	//generates uv coordinates for tiled texture settings
	for (int i = 0; i <= this->numTiles; i++) {
		for (int j = 0; j <= this->numTiles; j++) {
			this->uvCoordinates.push_back(glm::vec2((GLfloat)i, (GLfloat)j));
		}
	}

	glGenBuffers(1, &this->uv_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->uv_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->uvCoordinates.size() * sizeof(glm::vec2), &this->uvCoordinates.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Water::buildIndexEBO()
{
	for (int i = 0; i < this->numTiles; i++) {
		for (int j = 0; j < this->numTiles; j++) {
			GLuint point = i * (this->numTiles + 1) + j;

			//first half triangle
			indicies.push_back(point);
			indicies.push_back(point + (this->numTiles + 1));
			indicies.push_back(point + (this->numTiles + 1) + 1);
			//second half triangle
			indicies.push_back(point);
			indicies.push_back(point + (this->numTiles + 1) + 1);
			indicies.push_back(point + 1);
		}
	}
	glGenBuffers(1, &index_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), &indicies.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Water::buildVAO()
{
	//Bind VAO
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	//Register Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Register Normals Buffer
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	//Register UV Buffer
	glBindBuffer(GL_ARRAY_BUFFER, uv_VBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	//Register any other VBOs

	//Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_EBO);

	//Unbind VBO & VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Water::buildTexture(){
	char * imgLocation = "Images/water.jpg"; // seems to look more wavy

	glGenTextures(1, &waterTexture);
	glBindTexture(GL_TEXTURE_2D, waterTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image(imgLocation, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}
