#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
bool loadYJ(std::string path,
	//passed by reference
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec3>& out_normals) {
	//how to construct indicies for EBO..?
	//maybe mix with assimp in usage as a quick hack
	std::cout << "Loading .yj file : " << path << "\n";
	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec3> normals;
	std::ifstream infile(path);
	glm::vec3 vec(0.0f);
	char header;
	float x, y, z;
	while (infile >> header >> x >> y >> z) {
		if (header == 'v') {
			vec = glm::vec3(x, y, z);
			//vertices.push_back(vec);
			out_vertices.push_back(vec);
		}
		else if (header == 'n') {
			vec = glm::vec3(x, y, z);
			//normals.push_back(vec);
			out_normals.push_back(vec);
		}
		else return false;
	}

	//out_vertices=vertices;
	//out_normals=normals;

	return true;
}
bool loadNormalsYJ(std::string path,
	//passed by reference
	std::vector<glm::vec3>& out_normals) {
	//how to construct indicies for EBO..?
	//maybe mix with assimp in usage as a quick hack
	std::cout << "Loading .yj file : " << path << "\n";
	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec3> normals;
	std::ifstream infile(path);
	glm::vec3 vec(0.0f);
	char header;
	float x, y, z;
	while (infile >> header >> x >> y >> z) {
		if (header == 'v') {
			//vec = glm::vec3(x, y, z);
			//vertices.push_back(vec);
			//out_vertices.push_back(vec);
		}
		else if (header == 'n') {
			vec = glm::vec3(x, y, z);
			//normals.push_back(vec);
			out_normals.push_back(vec);
		}
		else return false;
	}

	//out_vertices=vertices;
	//out_normals=normals;

	return true;
}
class YJ {
public:
	GLuint VAO, positionBuffer, normalBuffer, textureBuffer, smoothedNormalsBuffer; //doesnt utilize EBO for now
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoordinates;
	std::string path;
	std::vector<glm::vec3> smoothedNormals[20];
	//std::vector<glm::vec3[20]> smoothedNormalsResized; -> YOU CAN'T MAKE A VECTOR OF PLAIN ARRAYS
	glm::vec4* smoothedNormalsSingleArr;
	bool isSet = false;
	YJ(std::string path ) {
		this->path = path;
		loadYJ(path);
		loadSmoothedNormals();
		//setupYJ(vertices, normals);
	}
	bool loadYJ(std::string path) {
		//how to construct indicies for EBO..?
		//maybe mix with assimp in usage as a quick hack
		std::cout << "Loading .yj file : " << path << "\n";
		//std::vector<glm::vec3> vertices;
		//std::vector<glm::vec3> normals;
		std::ifstream infile(path);
		glm::vec3 vec(0.0f);
		char header;
		float x, y, z;
		while (infile >> header >> x >> y >> z) {
			if (header == 'v') {
				vec = glm::vec3(x, y, z);
				//vertices.push_back(vec);
				vertices.push_back(vec);
			}
			else if (header == 'n') {
				vec = glm::vec3(x, y, z);
				//normals.push_back(vec);
				normals.push_back(vec);
			}
			else return false;
		}

		//out_vertices=vertices;
		//out_normals=normals;

		return true;
	}
	//load preprocessed smoothed normals
	void loadSmoothedNormals() {
		//std::vector<glm::vec3> smoothedNormalsTemp ;
		std::string smoothedPath=this->path;
		smoothedPath.erase(smoothedPath.length() - 3, 3); //erases .yj, bad implementation tbh if the legth of the extension name changes it won't work
		for (int i = 0; i < 20; i++) {
			smoothedPath = smoothedPath + "_k" + std::to_string(i+1) + ".yj";
			if (!loadNormalsYJ(smoothedPath, this->smoothedNormals[i]))std::cout << "Failed to load smoothed normals at : " << smoothedPath << "\n";
			else std::cout << "Loaded smoothed normals "<<i<< " at " << smoothedPath << "\nFirst smoothed normal : " << this->smoothedNormals[i][0].x << ", " << this->smoothedNormals[i][0].y << ", " << this->smoothedNormals[i][0].z<<"\n"<<"Size : "<<smoothedNormals[i].size()<<"\n";
			if (i > 8)smoothedPath.erase(smoothedPath.length() - 7, 7); //erases _k(number i).yj, bad implementation tbh if the legth of the extension name changes it won't work
			else smoothedPath.erase(smoothedPath.length() - 6, 6);
		}
		//Transpose because of reasons
		//smoothedNormalsArr = new glm::vec3* [smoothedNormals[0].size()] ;
		smoothedNormalsSingleArr = new glm::vec4[smoothedNormals[0].size() * 20];
		for (int i = 0; i < smoothedNormals[0].size(); i++) {
			//smoothedNormalsArr[i] = new glm::vec3[20];
			for (int j = 0; j < 20; j++) {
				//smoothedNormalsArr[i][j]=smoothedNormals[j][i];
				smoothedNormalsSingleArr[i + j*smoothedNormals[0].size()] = glm::vec4(smoothedNormals[j][i],0.0);
			}
		}
	}
	void setupYJ() {
		glGenVertexArrays(1, &VAO); //vertex array object
		glGenBuffers(1, &positionBuffer); //vertex buffer object
		glGenBuffers(1, &normalBuffer); //vertex buffer object
		glGenBuffers(1, &textureBuffer); //vertex buffer object

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		//TODO : SEND SMOOTHED NORMALS TO SHADER
		//SSBO //smoothedNormalsBuffer is GLuint ID
		//glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(smoothedNormals[0])*20,smoothedNormals,GL_STATIC_DRAW);
		
		//Can't send std::vector or any other objects to glsl, they won't work.
		
		glGenBuffers(1, &smoothedNormalsBuffer); //vertex buffer object
		glBindBuffer(GL_SHADER_STORAGE_BUFFER,smoothedNormalsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 20*smoothedNormals[0].size()*sizeof(glm::vec4), smoothedNormalsSingleArr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,smoothedNormalsBuffer);
		
		//SSBO Debugging
		for (int i = 0; i < 20; i++) {
			glm::vec4 dat;
			//offset measured in bytes not indices
			for (int j = 0; j < 2; j++) {
				int offset = i * smoothedNormals[0].size() * sizeof(glm::vec4) + j * sizeof(glm::vec4);
				glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec4), &dat);
				std::cout << "At " << offset << " : " << dat.x << ", " << dat.y << "," << dat.z << "\n";
				offset = (i + 1) * smoothedNormals[0].size() * sizeof(glm::vec4) - (j+1) * sizeof(glm::vec4);
				glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(glm::vec4), &dat);
				std::cout << "At " << offset << " : " << dat.x << ", " << dat.y << "," << dat.z << "\n";
			}

		}
		//unbind
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
		return;
	}

	bool render(GLuint shader) {

		if (!isSet) { setupYJ(); isSet = true; }
		glUniform1i(glGetUniformLocation(shader, "size"), smoothedNormals[0].size());
		/*GLuint VAO, positionBuffer, normalBuffer; //doesnt utilize the EBO
		glGenVertexArrays(1, &VAO); //vertex array object
		glGenBuffers(1, &positionBuffer); //vertex buffer object
		glGenBuffers(1, &normalBuffer); //vertex buffer object

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3),&vertices[0],GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		*/
		glUseProgram(shader);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		//glBindVertexArray(0);

		return true;
	}
};
