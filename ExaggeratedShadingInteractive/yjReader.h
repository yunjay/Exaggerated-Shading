#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
int loadingScales = 20;
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
		else if (header == 'i') {
			//normals.push_back(vec);

		}
		else return false;
	}

	//out_vertices=vertices;
	//out_normals=normals;

	return true;
}
class YJ {
public:
	GLuint VAO, positionBuffer, normalBuffer, textureBuffer, smoothedNormalsBuffer, EBO, maxPD, minPD, maxCurv, minCurv; 
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<unsigned int> indices;
	std::string path, pdPath;
	std::vector<glm::vec3> smoothedNormals[20];
	//std::vector<glm::vec3[20]> smoothedNormalsResized; -> YOU CAN'T MAKE A VECTOR OF PLAIN ARRAYS
	glm::vec4* smoothedNormalsSingleArr;
	bool isSet = false;

	std::vector<glm::vec3> maxPDs, minPDs;
	std::vector<float> maxCurvs, minCurvs;

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
			else if (header == 'i') {
				//normals.push_back(vec);
				indices.push_back((unsigned int)x);
			}
			else return false;
		}

		//out_vertices=vertices;
		//out_normals=normals;
		std::cout << "Size of vertices : " << vertices.size() << "\n";
		std::cout << "Size of normals : " << normals.size() << "\n";
		std::cout << "Size of indices : " << indices.size() << "\n";
		return true;
	}
	//load preprocessed smoothed normals
	void loadSmoothedNormals() {
		//std::vector<glm::vec3> smoothedNormalsTemp ;
		std::string smoothedPath=this->path;
		smoothedPath.erase(smoothedPath.length() - 3, 3); //erases .yj, bad implementation tbh if the legth of the extension name changes it won't work
		for (int i = 0; i < loadingScales; i++) {
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
			for (int j = 0; j < loadingScales; j++) {
				//smoothedNormalsArr[i][j]=smoothedNormals[j][i];
				smoothedNormalsSingleArr[i + j*smoothedNormals[0].size()] = glm::vec4(smoothedNormals[j][i],0.0);
			}
		}
	}
	bool loadPD() {
		//how to construct indicies for EBO..?
		//maybe mix with assimp in usage as a quick hack
		std::cout << "Loading .pd file : " << pdPath << "\n";
		//std::vector<glm::vec3> vertices;
		//std::vector<glm::vec3> normals;
		std::ifstream infile(pdPath);
		glm::vec3 vec(0.0f);
		std::string header;
		float x, y, z;
		while (infile >> header >> x >> y >> z) {
			if (header == "b") {
				vec = glm::vec3(x, y, z);
				maxPDs.push_back(vec);
			}
			else if (header == "s") {
				vec = glm::vec3(x, y, z);
				minPDs.push_back(vec);
			}
			else if (header == "bk") {
				maxCurvs.push_back(x);
			}
			else if (header == "sk") {
				minCurvs.push_back(x);
			}
			else return false;
		}
		std::cout << "Number of principal directions : " <<maxPDs.size()+minPDs.size() <<"\n";;
		//out_vertices=vertices;
		//out_normals=normals;

		return true;
	}

	void setup() {
		std::cout << "Setting up buffers.\n";

		glGenVertexArrays(1, &VAO); //vertex array object
		glGenBuffers(1, &positionBuffer); //vertex buffer object
		glGenBuffers(1, &normalBuffer); //vertex buffer object
		glGenBuffers(1, &textureBuffer); //vertex buffer object
		
		glGenBuffers(1, &EBO); 

		glGenBuffers(1, &maxPD);
		glGenBuffers(1, &minPD);
		glGenBuffers(1, &maxCurv);
		glGenBuffers(1, &minCurv);

		//VAO  
		glBindVertexArray(VAO); 

		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		
		// maxPD, minPD, maxCurv, minCurv

		glBindBuffer(GL_ARRAY_BUFFER, maxPD);
		glBufferData(GL_ARRAY_BUFFER, maxPDs.size() * sizeof(glm::vec3), &maxPDs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, minPD);
		glBufferData(GL_ARRAY_BUFFER, minPDs.size() * sizeof(glm::vec3), &minPDs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, maxCurv);
		glBufferData(GL_ARRAY_BUFFER, maxCurvs.size() * sizeof(float), &maxCurvs[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, minCurv);
		glBufferData(GL_ARRAY_BUFFER, minCurvs.size() * sizeof(float), &minCurvs[0], GL_STATIC_DRAW);



		//EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);



		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, maxPD);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, minPD);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, maxCurv);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, minCurv);
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//SEND SMOOTHED NORMALS TO SHADER
		//SSBO //smoothedNormalsBuffer is GLuint ID
		//glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(smoothedNormals[0])*20,smoothedNormals,GL_STATIC_DRAW);
		
		//Can't send std::vector or any other objects to glsl, they won't work.
		
		glGenBuffers(1, &smoothedNormalsBuffer); //vertex buffer object
		glBindBuffer(GL_SHADER_STORAGE_BUFFER,smoothedNormalsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 20*smoothedNormals[0].size()*sizeof(glm::vec4), smoothedNormalsSingleArr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,7,smoothedNormalsBuffer);
		
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);

		std::cout << "Ready to render.\n";

		return;
	}

	bool render(GLuint shader) {

		if (!isSet) { setup(); isSet = true; }

		glUniform1i(glGetUniformLocation(shader, "size"), smoothedNormals[0].size());
		
		glUseProgram(shader);
		glBindVertexArray(VAO);
		
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);


		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		//glBindVertexArray(0);

		return true;
	}
};

		