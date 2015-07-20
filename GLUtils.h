#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vertex.h"
#include <vector>

namespace glutils {

void drawCircle(float r1, float r2, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawQuad(float w, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawSphere(float radius, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);
void drawCylinder(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices);

}
