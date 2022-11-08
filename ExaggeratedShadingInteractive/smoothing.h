#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

#include <vector>
#include <string>
#include <math.h>
#include <chrono>

#include <ppl.h>


//#include "model.h"
using std::vector;
using std::string;
using std::cout;
//inline is best for small functions that are called often. Operator-daddies I'll call them
//distance squared
inline float dist2(const glm::vec3& p1, const glm::vec3& p2) {
	float d = glm::distance(p1, p2);
	return d * d;
}
inline float invsigma2(float sigma) {
	return 1 / sigma / sigma;
}
//gaussian weight
inline float wt(const glm::vec3& p1, const glm::vec3& p2, float invsigma2)
{
	//exponent to e (-x^2/2sig^2)
	float d2 = -0.5f * invsigma2 * dist2(p1, p2);
	//as we're going to add to a weighted average, I don't think multiplied constants are relevant
	//so I'm excluding the 1/sigma*sqrt(2*PI) factor
	
	//I want dist > 3* sigma or 2*sigma
	//return (d2>=25.0f)? 0.0f:glm::exp(d2);
	return (d2 >= 9.0f) ? 0.0f : exp(d2);
	//return (d2 >= 16.0f) ? 0.0f : exp(-0.5f*d2); 
}
//computes "feature size" - median edge length -> first sigma
float featureSize(const vector<glm::vec3> vertices) {
	float size=0.0f;
	//Intuitive implemetation calculating model boundary box size
	float maxX=vertices[0].x,maxY = vertices[0].y,maxZ = vertices[0].z;
	float minX = vertices[0].x, minY = vertices[0].y, minZ = vertices[0].z;
	for (int i = 1; i < vertices.size(); i++) {
		(vertices[i].x > maxX) ? maxX = vertices[i].x : 0;
		(vertices[i].y > maxY) ? maxY = vertices[i].y : 0;
		(vertices[i].z > maxZ) ? maxZ = vertices[i].z : 0;
		(vertices[i].x < minX) ? minX = vertices[i].x : 0;
		(vertices[i].y < minY) ? minY = vertices[i].y : 0;
		(vertices[i].z < minZ) ? minZ = vertices[i].z : 0;
	}
	glm::vec3 diagonal(maxX-minX,maxY-minY,maxZ-minZ);
	float diagonalLength = glm::length(diagonal);
	//cout << "Diagonal length of box bounding model : "<<diagonalLength<<"\n";
	//arbitrary denominator
	size = diagonalLength/50.0f;
	//Better Implementation - SUB SAMPLING
	return size;
}
glm::vec3 center(const vector<glm::vec3> vertices) {
	//Intuitive implemetation calculating model boundary box size
	float maxX = vertices[0].x, maxY = vertices[0].y, maxZ = vertices[0].z;
	float minX = vertices[0].x, minY = vertices[0].y, minZ = vertices[0].z;
	for (int i = 1; i < vertices.size(); i++) {
		(vertices[i].x > maxX) ? maxX = vertices[i].x : 0;
		(vertices[i].y > maxY) ? maxY = vertices[i].y : 0;
		(vertices[i].z > maxZ) ? maxZ = vertices[i].z : 0;
		(vertices[i].x < minX) ? minX = vertices[i].x : 0;
		(vertices[i].y < minY) ? minY = vertices[i].y : 0;
		(vertices[i].z < minZ) ? minZ = vertices[i].z : 0;
	}
	return glm::vec3((maxX+minX)/2.0f, (maxY + minY) / 2.0f, (maxZ + minZ) / 2.0f);
}
void smooth(const vector<glm::vec3> normals, vector<glm::vec3>& smoothed, const float invsigma2 ) {
	cout << "Smoothing...\n";
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < normals.size(); i++) {
		if(i%1000==0)cout << "normal : " << i << "\n";
		float weightSum = 0.0f; //this init sets value ever loop (I forgot everything to do with c++)
		glm::vec3 sum(0.0f); //constructs to 0,0,0
		for (int j = 0; j < normals.size(); j++) {
			float weight = wt(normals[i],normals[j], invsigma2); 
			weightSum += weight; //sum of weights
			sum += weight*normals[j]; //add to vertex
		}
		//divide by total weight - can also refactor this to be 1...
		smoothed[i] = sum/weightSum;
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	cout << "Smoothing Done!\n"<<"Took : "<< elapsed_seconds.count() <<" seconds.\n";;
	return;
}
//using parallel_for from ppl.h
void smooth_parallel(const vector<glm::vec3> normals, vector<glm::vec3>& smoothed, const float invsigma2) {
	//vector<float> weights(normals.size());
	//weighted average of normals (gaussian)
	//TODO : OPTIMIZE
	//PARALLEL PROGRAMMING / MULTITHREADING -> CUDA or C++ standard... 
	//Or make data a 3D array and use indexing for getting neighbors
	//As in compute distance once, place in a 3D-like data structure to get neighbors
	cout << "Smoothing using parallel computation...\n";
	auto start = std::chrono::high_resolution_clock::now();
	//this for loop is a function
	/*
	for (int i = 0; i < normals.size(); i++) {
		if (i % 1000 == 0)cout << "normal : " << i << "\n";
		float weightSum = 0.0f; //this init sets value ever loop (I forgot everything to do with c++)
		glm::vec3 sum(0.0f); //constructs to 0,0,0
		concurrency::parallel_for(int(0), (int)normals.size(), [&](int j) {
			float weight = wt(normals[i], normals[j], invsigma2);
			weightSum += weight; //sum of weights
			sum += weight * normals[j]; //add to vertex
		});
		//divide by total weight - can also refactor this to be 1...
		smoothed[i] = sum / weightSum;
	}
	*/
	concurrency::parallel_for(size_t(0), normals.size(), [&](size_t i) {
		if (i % 1000 == 0)cout << "normal : " << i << "\n";
		float weightSum = 0.0f; //this init sets value ever loop (I forgot everything to do with c++)
		glm::vec3 sum(0.0f); //constructs to 0,0,0
		for (int j = 0; j < normals.size(); j++) {
			float weight = wt(normals[i], normals[j], invsigma2);
			weightSum += weight; //sum of weights
			sum += weight * normals[j]; //add to vertex
		}
		//divide by total weight - can also refactor this to be 1...
		smoothed[i] = sum / weightSum;
		});

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	cout << "Parallelly Computed Smoothing Done !\n" << "Took : " << elapsed_seconds.count() << " seconds.\n";;
	return;
}
bool writeFile(vector<glm::vec3> vertices, vector<glm::vec3> normals, string fileName) {
	std::ofstream file(fileName);
	if (vertices.size() != normals.size()) { cout << "vertices size != normals size"; return false; }
	for (int i = 0; i < vertices.size(); i++) {
		file << "v ";
		file << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";
		file << "n ";
		file << normals[i].x << " " << normals[i].y << " " << normals[i].z << "\n";
	}
	file.close();
	return true;
}
/*
int main() {
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec3> smoothedNormals;
	//string path = "C:/Users/lab/Desktop/yj/yjFileCreator/alduin/alduin-dragon.obj"; //string.data() for char *
	string path = "C:/Users/lab/Desktop/yj/yjFileCreator/models/stanford-bunny.obj";
	//string path = "C:/Users/lab/Desktop/yj/yjFileCreator/models/brain-simple-mesh.obj";
	
	if (!loadOBJ(path.data(), vertices, normals)) { cout << "obj failed to load"; return -1; } //load vertices, normals
	//next sigma = sigma * sqrt2
	const float freqMult = glm::sqrt(2);
	//.95 reliability?
	float sigma = 0.4*featureSize(vertices);
	
	//now write obj and smoothed normals to a file
	cout << "Saving .obj as .yj\n";
	char *pname=new char(path.size()); 
	_splitpath(path.data(), NULL,NULL,pname,NULL);
	string name(pname);
	//delete pname;
	writeFile(vertices, normals, name + ".yj");

	smoothedNormals.resize(normals.size());
	for (int i = 1; i < 21; i++) {
		cout << "Saving smoothed normal object number : " << i << "\n";
		//smooth(normals,smoothedNormals,invsigma2(sigma));
		smooth_parallel(normals, smoothedNormals, invsigma2(sigma));
		writeFile(vertices, smoothedNormals, name + "_k" + std::to_string(i)+".yj");
		sigma *= freqMult;
		normals = smoothedNormals; 
		std::fill(smoothedNormals.begin(),smoothedNormals.end(),glm::vec3(0.0f));
	}


	return 0;
}
//mult[i] = pow(data->sigmas[i], -freqdep);
//initial sigma in paper : 
//float sigma = 0.4f * mesh->feature_size();
//then multiplied by sqrt(2) every scale
//alduin model is 100k vertices, maybe use bunny?
*/