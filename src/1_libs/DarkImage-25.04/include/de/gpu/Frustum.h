#pragma once 
#include <de/gpu/Camera.h>
#include <array>

namespace de {


} // end namespace de	

/*
Camera

glm::vec4 ndcCorners[8] = {
    {-1, -1, -1, 1}, 
	{1, -1, -1, 1}, 
	{-1, 1, -1, 1}, 
	{1, 1, -1, 1},
    {-1, -1,  1, 1}, 
	{1, -1,  1, 1}, 
	{-1, 1,  1, 1}, 
	{1, 1,  1, 1}
};

glm::vec3 worldCorners[8];
for (int i = 0; i < 8; i++) {
    glm::vec4 worldPos = glm::inverse(VP) * ndcCorners[i];
    worldCorners[i] = worldPos.xyz / worldPos.w; // Convert from homogeneous to world space
}

glm::vec3 left_down_near  = worldCorners[0];
glm::vec3 right_down_near = worldCorners[1];
glm::vec3 left_up_near    = worldCorners[2];
glm::vec3 right_up_near   = worldCorners[3];

glm::vec3 left_down_far   = worldCorners[4];
glm::vec3 right_down_far  = worldCorners[5];
glm::vec3 left_up_far     = worldCorners[6];
glm::vec3 right_up_far    = worldCorners[7];

glBegin(GL_LINES);
for (int i = 0; i < 4; i++) {
    glVertex3fv(&worldCorners[i].x);
    glVertex3fv(&worldCorners[i + 4].x); // Connect near and far plane
}

int edges[12][2] = { {0,1}, {1,3}, {3,2}, {2,0}, {4,5}, {5,7}, {7,6}, {6,4}, {0,4}, {1,5}, {2,6}, {3,7} };
for (auto& e : edges) {
    glVertex3fv(&worldCorners[e[0]].x);
    glVertex3fv(&worldCorners[e[1]].x);
}
glEnd();


*/
