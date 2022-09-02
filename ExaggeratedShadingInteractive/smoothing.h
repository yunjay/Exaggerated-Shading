
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
//invsigma2 is inverse of sigma squared
// Approximation to Gaussian...  Used in filtering
inline float dist2(const glm::vec3&p1, const glm::vec3&p2) {
	float d= glm::distance(p1, p2)*;
	return d * d;
}
inline float wt(const glm::vec3& p1, const glm::vec3& p2, float invsigma2)
{
	float d2 = invsigma2 * dist2(p1, p2);
	//return (d2 >= 4.0f) ? 0.0f : 1.0f - d2 * (0.5f - d2 * 0.0625f);
	return (d2 >= 6.25f) ? 0.0f : 1.0f - d2 * (0.32f - d2 * 0.0256f);
	//return (d2 >= 9.0f) ? 0.0f : exp(-0.5f*d2);
	//return (d2 >= 25.0f) ? 0.0f : exp(-0.5f*d2);
}
inline float wt(const TriMesh* themesh, int v1, int v2, float invsigma2)
{
	return wt(themesh->vertices[v1], themesh->vertices[v2], invsigma2);
}

//smooth normals
void smoothNormals(Mesh mesh, float sigma) {

	std::cout << "Smoothing Normals\n";
	std::time_t *t;
	time(t);

	std::cout << "Filtering took"<<time(NULL)-*t<< " seconds.\n";
	return;
}
