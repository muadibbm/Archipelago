#include "Terrain.h"
#include "NoiseGeneration.h"



Terrain::Terrain(unsigned int seed) : width(TERRAIN_WIDTH), length(TERRAIN_LENGTH)
{
	std::cout << "Generating Terrain" << std::endl;
	srand(seed);

	//build vertex vbo
	buildVertexVBO();
	buildNormalsVBO();
	buildUVVBO();
	//build index EBO
	buildIndexEBO();
	//build VAO
	buildVAO();

	buildIslandTexture();
}


void Terrain::draw()
{
	glBindTexture(GL_TEXTURE_2D, islandTexture);
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indicies.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Terrain::~Terrain() {}


void Terrain::buildVertexVBO()
{
	GLfloat halfWidth = this->width / 2;
	GLfloat halfLength = this->length / 2;

	//Creates a plane at y=0, with given width and length, centered at the origin
	for (int l = 0; l < this->length; l++) {
		for (int w = 0; w < this->width; w++) {
			this->vertices.push_back(glm::vec3((GLfloat)w - halfWidth, 0.0f, (GLfloat)l - halfLength));
		}
	}

	//Modify y values with perlin noise?
	this->useNoise();

	//Modify y values with island mask
	this->islandMask();

	glGenBuffers(1, &this->vertex_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::buildNormalsVBO()
{
	glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 U, UL, R, L, D, DR;
	int vertex;

	//Find normals for each vertex
	for (int l = 0; l < this->length; l++) {
		for (int w = 0; w < this->width; w++) {
			vertex = l * this->width + w;

			//there are faces above vertex
			if (l != 0) {
				U = glm::vec3(this->vertices[vertex - this->width] - this->vertices[vertex]);
				//there are faces to the left
				if (w != 0) {
					L = glm::vec3(this->vertices[vertex - 1] - this->vertices[vertex]);
					UL = glm::vec3(this->vertices[vertex - this->width - 1] - this->vertices[vertex]);

					normal += glm::normalize(glm::cross(UL, L));
					normal += glm::normalize(glm::cross(U, UL));
				}
				//there are faces to the right
				if (w != this->width - 1) {
					R = glm::vec3(this->vertices[vertex + 1] - this->vertices[vertex]);

					normal += glm::normalize(glm::cross(R, U));
				}
			}
			//there are faces below vertex
			if (l != this->length - 1) {
				D = glm::vec3(this->vertices[vertex + this->width] - this->vertices[vertex]);
				//there are faces to the left
				if (w != 0) {
					L = glm::vec3(this->vertices[vertex - 1] - this->vertices[vertex]);

					normal += glm::normalize(glm::cross(L, D));
				}
				//there are faces to the right
				if (w != this->width - 1) {
					R = glm::vec3(this->vertices[vertex + 1] - this->vertices[vertex]);
					DR = glm::vec3(this->vertices[vertex + this->width + 1] - this->vertices[vertex]);

					normal += glm::normalize(glm::cross(D, DR));
					normal += glm::normalize(glm::cross(DR, R));
				}
			}
			normal = normal / 6.0f;
			this->normals.push_back(glm::normalize(normal));
		}
	}

	glGenBuffers(1, &this->normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::buildUVVBO() {
	for (int l = 0; l < this->length; l++) {
		for (int w = 0; w < this->width; w++) {
			this->uvCoordinates.push_back(glm::vec2((GLfloat)l, (GLfloat)w));
		}
	}

	glGenBuffers(1, &this->uv_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->uv_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->uvCoordinates.size() * sizeof(glm::vec2), &this->uvCoordinates.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::buildIndexEBO()
{
	for (int l = 0; l < this->length - 1; l++) {
		for (int w = 0; w < this->width - 1; w++) {
			GLuint point = l*width + w;

			//first half triangle
			indicies.push_back(point);
			indicies.push_back(point + (this->width));
			indicies.push_back(point + (this->width) + 1);

			//second half triangle
			indicies.push_back(point);
			indicies.push_back(point + (this->width) + 1);
			indicies.push_back(point + 1);
		}
	}

	glGenBuffers(1, &this->index_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indicies.size() * sizeof(GLuint), &this->indicies.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::buildVAO()
{
	//Bind VAO
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	//Register Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Register Normals Buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->normals_VBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	//Register UV Buffer
	glBindBuffer(GL_ARRAY_BUFFER, uv_VBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	//Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_EBO);

	//Unbind VBO & VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//adding texture to the island
void Terrain::buildIslandTexture() {
	char * imgLocation = "Images/islandText.jpg"; // seems to look more wavy

	glGenTextures(1, &islandTexture);
	glBindTexture(GL_TEXTURE_2D, islandTexture);

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


//modified implementation of concept, Reference: https://www.reddit.com/r/gamedev/comments/1g4eae/need_help_generating_an_island_using_perlin_noise/?st=iuritk3l&sh=594f7e28
void Terrain::islandMask()
{
	const GLfloat MAGNITUDE = 15.0f; //20.0f
	const GLfloat DISTANCE_TO_ZERO = 100;

	int numCentrePoints;
	int centerXCoord, centerZCoord;
	GLfloat vertex, distance;

	//generate # of centre points
		//rule: Any value between 1-5 inclusive
	numCentrePoints = 1 + rand() % 10;

	//generate centre point position for each # of points
		//rule: cannot select points within X distance of the edge
		//x coordinate = X + random # between 0 and width - 2X
		//z coordinate = X + random # between 0 and length - 2X

	for (int i = 0; i < numCentrePoints; i++) {
		centerXCoord = WATER_BORDER + rand() % (this->width - 2 * WATER_BORDER);
		centerZCoord = WATER_BORDER + rand() % (this->length - 2 * WATER_BORDER);

		//Adjust height values in array for each centre point
		//Add distance * magnitude to the height values (y) of all the verticies vector/array
		for (int l = 0; l < this->length; l++) {
			for (int w = 0; w < this->width; w++) {
				vertex = l * this->width + w;
				distance = std::sqrt(std::pow(centerXCoord - w, 2) + std::pow(centerZCoord - l, 2));

				//clamp if outside sloping zone;
				if (distance > DISTANCE_TO_ZERO)
					distance = DISTANCE_TO_ZERO;

				this->vertices[vertex].y += ((DISTANCE_TO_ZERO - distance)/DISTANCE_TO_ZERO) * MAGNITUDE;
			}
		}
	}
}

//function that goes through the  vertices length and pulls the x and z from each vec3
//and then call the generateHeight(x , z) and then assign it to the y in the vec3 of vertices
//place thos into vector
void Terrain::useNoise() {

	NoiseGeneration noise;
	int i = 0;
	float temp1, temp2;
	temp1= 0;
	temp2=0;
	
	for (int l = 0; l < this->length; l++) {
		for (int w = 0; w < this->width; w++) {

		/*
			//if (-x, -z)
			if ((vertices[i].x) < 0 && (vertices[i].z) < 0) //used to be || //**new
			{

				temp1 = vertices[i].x + (width/2);
				temp2 = vertices[i].z + (length/2);

				vertices[i].y = noise.generateHeight(temp1, temp2);

				i++;
			}
			//if (x, -z)
			else if ((vertices[i].x) > 0 && (vertices[i].z) < 0) //**new
			{

				temp1 = vertices[i].x - (width/2);
				temp2 = vertices[i].z + (length/2);

				vertices[i].y = (noise.generateHeight(temp1, temp2));

				i++;

			}
			//if (-x, z)
			else if ((vertices[i].x) < 0 && (vertices[i].z) > 0) //**new
			{
				temp1 = vertices[i].x + (width/2);
				temp2 = vertices[i].z - (length/2);

				vertices[i].y = (noise.generateHeight(temp1, temp2));

				i++;
			}
			//if (x,z)
			else

			{
				vertices[i].y = (noise.generateHeight(vertices[i].x, vertices[i].z));
				i++;
			}
		*/
			//if (-x, -z)
			if ((vertices[i].x) <= 0 && (vertices[i].z ) <= 0) //used to be || //**new
			{
				
				temp1 = vertices[i].x + (width); 
				temp2 = vertices[i].z + (length);

				vertices[i].y = noise.generateHeight(temp1, temp2);

				i++;
			}
			//if (x, -z)
			else if((vertices[i].x) > 0 && (vertices[i].z) <= 0) //**new
			{ 

				temp1 = vertices[i].x + width;
				temp2 = vertices[i].z + (length);

				vertices[i].y = (noise.generateHeight(temp1, temp2));

				i++;
			
			} 
			//if (-x, z)
			else if ((vertices[i].x) <= 0 && (vertices[i].z) > 0) //**new
			{
				temp1 = vertices[i].x + (width);
				temp2 = vertices[i].z + length;

				vertices[i].y = (noise.generateHeight(temp1, temp2));

				i++;
			}
			//if (x,z)
			else

			{
				vertices[i].y = (noise.generateHeight(vertices[i].x, vertices[i].z));
				i++;
			}
		

		}
	}
}
