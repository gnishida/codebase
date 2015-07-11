#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.h"
#include <vector>

namespace glutils {

void drawQuad(const glm::vec3& c, float l, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawSphere(const glm::vec3& c, float r, const glm::vec3& color);
void drawSphere(const glm::vec3& c, float r, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawCylinder(const glm::vec3& p, float h, float r, const glm::vec3& color);
void drawCylinder(const glm::vec3& p, float h, float r, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawCone(const glm::vec3& p, float h, float r1, float r2, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);

}
