#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.h"
#include <vector>

namespace glutils {

void drawCircle(float r1, float r2, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawQuad(float w, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawQuad(float w, float h, const glm::vec3& t1, const glm::vec3& t2, const glm::vec3& t3, const glm::vec3& t4, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawBox(float length_x, float length_y, float length_z, glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawSphere(float radius, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawCylinderY(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawCylinderZ(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawArrow(float radius, float length, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawAxes(float radius, float length, const glm::mat4& mat, std::vector<Vertex>& vertices);

float deg2rad(float degree);

}
