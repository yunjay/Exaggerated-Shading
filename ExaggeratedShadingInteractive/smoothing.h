
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>

#include "mesh.h"
#include "model.h"
//point = vec3
inline float dist2(const glm::vec3&p1, const glm::vec3&p2) {
	float d= glm::distance(p1, p2)*;
	return d * d;
}
//invsigma2 is inverse of sigma squared
// Approximation to Gaussian...  Used in filtering
// weight
inline float wt(const glm::vec3& p1, const glm::vec3& p2, float invsigma2)
{
	float d2 = invsigma2 * dist2(p1, p2);
	//return (d2 >= 4.0f) ? 0.0f : 1.0f - d2 * (0.5f - d2 * 0.0625f);
	return (d2 >= 6.25f) ? 0.0f : 1.0f - d2 * (0.32f - d2 * 0.0256f);
	//return (d2 >= 9.0f) ? 0.0f : exp(-0.5f*d2);
	//return (d2 >= 25.0f) ? 0.0f : exp(-0.5f*d2);
}
/*
//overloaded for mesh 
inline float wt(const TriMesh* themesh, int v1, int v2, float invsigma2)
{
	return wt(themesh->vertices[v1], themesh->vertices[v2], invsigma2);
}
*/

void calculateWeights(Model *model){
	//for every mesh in model
	for(int i =0; i<model->vertexPositions.size();i++){
		for(int j=i; j<model->meshes[i].vertices.size();j++){
			
		}
	}
}

//smooth normals for some scales
void smoothNormals(Mesh mesh, float sigma) {

	std::cout << "Smoothing Normals\n";
	std::time_t *t;
	time(t);

	std::cout << "Filtering took"<<time(NULL)-*t<< " seconds.\n";
	return;
}
//calculate
void calculateNormals() {

}
// problems to solve
// 1. calculating smoothed normals for eacy scales and saving to file (too large for local variables) up to 21 ks probably preprocessed.
// Likely should make a different solution for preprocessing normals
// 2. adding up everything according to contribution factor ki inside the shader
// 3. take the user-variables as uniforms probably
// i dont think using a geometry shader to solve in real time is smart, preprocessing seems like the best answer. The files get big af though fr

