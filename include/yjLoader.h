#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>

bool loadYJ(std::string path,
	//passed by reference
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec3>& out_normals) {
	//how to construct indicies for EBO..?
	//maybe mix with assimp in usage as a quick hack
	std::cout << "Loading .yj file : "<<path<<"\n";
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