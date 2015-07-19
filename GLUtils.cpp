#include "GLUtils.h"
#include <QGLWidget>

#ifndef M_PI
#define M_PI	3.14159265359
#endif

namespace glutils {

void drawCircle(float r1, float r2, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	int slices = 12;

	glm::vec4 p1(0, 0, 0, 1);
	glm::vec4 n(0, 0, 1, 0);

	p1 = mat * p1;
	n = mat * n;

	for (int i = 0; i < slices; ++i) {
		float theta1 = (float)i / slices * M_PI * 2.0f;
		float theta2 = (float)(i + 1) / slices * M_PI * 2.0f;

		glm::vec4 p2(cosf(theta1) * r1, sinf(theta1) * r2, 0, 1);
		glm::vec4 p3(cosf(theta2) * r1, sinf(theta2) * r2, 0, 1);

		p2 = mat * p2;
		p3 = mat * p3;
		
		vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), color));
		vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n), color));
		vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), color));
	}
}

void drawQuad(float l, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	glm::vec4 p1(-l * 0.5, -l * 0.5, 0, 1);
	glm::vec4 p2(l * 0.5, -l * 0.5, 0, 1);
	glm::vec4 p3(l * 0.5, l * 0.5, 0, 1);
	glm::vec4 p4(-l * 0.5, l * 0.5, 0, 1);
	glm::vec4 n(0, 0, 1, 0);

	p1 = mat * p1;
	p2 = mat * p2;
	p3 = mat * p3;
	p4 = mat * p4;
	n = mat * n;

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), color));
	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n), color));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), color));

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), color));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), color));
	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n), color));
}

void drawSphere(float radius, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	int slices = 12;
	int stacks = 6;

	for (int i = 0; i < stacks; ++i) {
		float phi1 = M_PI * (float)i / stacks - M_PI * 0.5;
		float phi2 = M_PI * (float)(i + 1) / stacks - M_PI * 0.5;
		float r1 = cosf(phi1) * radius;
		float r2 = cosf(phi2) * radius;

		for (int j = 0; j < slices; ++j) {
			float theta1 = M_PI * 2.0 * (float)j / slices;
			float theta2 = M_PI * 2.0 * (float)(j + 1) / slices;

			glm::vec4 p1(cosf(theta1) * r1, sinf(theta1) * r1, sinf(phi1) * radius, 1);
			glm::vec4 p2(cosf(theta2) * r1, sinf(theta2) * r1, sinf(phi1) * radius, 1);
			glm::vec4 p3(cosf(theta2) * r2, sinf(theta2) * r2, sinf(phi2) * radius, 1);
			glm::vec4 p4(cosf(theta1) * r2, sinf(theta1) * r2, sinf(phi2) * radius, 1);

			glm::vec4 n1(cosf(phi1) * cosf(theta1), cosf(phi1) * sinf(theta1), sinf(phi1), 0);
			glm::vec4 n2(cosf(phi1) * cosf(theta2), cosf(phi1) * sinf(theta2), sinf(phi1), 0);
			glm::vec4 n3(cosf(phi2) * cosf(theta2), cosf(phi2) * sinf(theta2), sinf(phi2), 0);
			glm::vec4 n4(cosf(phi2) * cosf(theta1), cosf(phi2) * sinf(theta1), sinf(phi2), 0);

			p1 = mat * p1;
			p2 = mat * p2;
			p3 = mat * p3;
			p4 = mat * p4;
			n1 = mat * n1;
			n2 = mat * n2;
			n3 = mat * n3;
			n4 = mat * n4;

			vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n1), color));
			vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n2), color));
			vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n3), color));

			vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n1), color));
			vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n3), color));
			vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n4), color));
		}
	}
}

/**
 * 高さ h、底面の半径 r1、上面の半径 r2の円錐を描画する。
 */
void drawCylinder(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	int slices = 12;

	float phi = atan2(radius1 - radius2, h);

	for (int i = 0; i < slices; ++i) {
		float theta1 = M_PI * 2.0 * (float)i / slices;
		float theta2 = M_PI * 2.0 * (float)(i + 1) / slices;

		glm::vec4 p1(cosf(theta1) * radius1, sinf(theta1) * radius1, 0, 1);
		glm::vec4 p2(cosf(theta2) * radius1, sinf(theta2) * radius1, 0, 1);
		glm::vec4 p3(cosf(theta2) * radius2, sinf(theta2) * radius2, h, 1);
		glm::vec4 p4(cosf(theta1) * radius2, sinf(theta1) * radius2, h, 1);
		glm::vec4 n1(cosf(theta1) * cosf(phi), sinf(theta1) * cosf(phi), sinf(phi), 0);
		glm::vec4 n2(cosf(theta2) * cosf(phi), sinf(theta2) * cosf(phi), sinf(phi), 0);

		p1 = mat * p1;
		p2 = mat * p2;
		p3 = mat * p3;
		p4 = mat * p4;
		n1 = mat * n1;
		n2 = mat * n2;

		vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n1), color));
		vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n2), color));
		vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n2), color));

		vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n1), color));
		vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n2), color));
		vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n1), color));
	}
}

}
