#include "GLUtils.h"
#include <QGLWidget>

#ifndef M_PI
#define M_PI	3.14159265359
#endif

namespace glutils {

void drawQuad(const glm::vec3& c, float l, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	glm::vec4 p1(c.x - l * 0.5, c.y - l * 0.5, c.z, 1);
	glm::vec4 p2(c.x + l * 0.5, c.y - l * 0.5, c.z, 1);
	glm::vec4 p3(c.x + l * 0.5, c.y + l * 0.5, c.z, 1);
	glm::vec4 p4(c.x - l * 0.5, c.y + l * 0.5, c.z, 1);
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

void drawSphere(const glm::vec3& c, float r, const glm::vec3& color) {
	int slices = 12;
	int stacks = 6;

	glBegin(GL_QUADS);
	for (int i = 0; i < stacks; ++i) {
		float phi1 = M_PI * (float)i / stacks - M_PI * 0.5;
		float phi2 = M_PI * (float)(i + 1) / stacks - M_PI * 0.5;
		float radius1 = cosf(phi1) * r;
		float radius2 = cosf(phi2) * r;

		for (int j = 0; j < slices; ++j) {
			float theta1 = M_PI * 2.0 * (float)j / slices;
			float theta2 = M_PI * 2.0 * (float)(j + 1) / slices;

			glNormal3f(cosf(phi1) * cosf(theta1), cosf(phi1) * sinf(theta1), sinf(phi1));
			glVertex3f(cosf(theta1) * radius1, sinf(theta1) * radius1, sinf(phi1) * r);
			glNormal3f(cosf(phi1) * cosf(theta2), cosf(phi1) * sinf(theta2), sinf(phi1));
			glVertex3f(cosf(theta2) * radius1, sinf(theta2) * radius1, sinf(phi1) * r);
			glNormal3f(cosf(phi2) * cosf(theta2), cosf(phi2) * sinf(theta2), sinf(phi2));
			glVertex3f(cosf(theta2) * radius2, sinf(theta2) * radius2, sinf(phi2) * r);
			glNormal3f(cosf(phi2) * cosf(theta1), cosf(phi2) * sinf(theta1), sinf(phi2));
			glVertex3f(cosf(theta1) * radius2, sinf(theta1) * radius2, sinf(phi2) * r);
		}
	}
	glEnd();
}

void drawSphere(const glm::vec3& c, float r, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	int slices = 12;
	int stacks = 6;

	for (int i = 0; i < stacks; ++i) {
		float phi1 = M_PI * (float)i / stacks - M_PI * 0.5;
		float phi2 = M_PI * (float)(i + 1) / stacks - M_PI * 0.5;
		float radius1 = cosf(phi1) * r;
		float radius2 = cosf(phi2) * r;

		for (int j = 0; j < slices; ++j) {
			float theta1 = M_PI * 2.0 * (float)j / slices;
			float theta2 = M_PI * 2.0 * (float)(j + 1) / slices;

			glm::vec4 p1(cosf(theta1) * radius1, sinf(theta1) * radius1, sinf(phi1) * r, 1);
			glm::vec4 p2(cosf(theta2) * radius1, sinf(theta2) * radius1, sinf(phi1) * r, 1);
			glm::vec4 p3(cosf(theta2) * radius2, sinf(theta2) * radius2, sinf(phi2) * r, 1);
			glm::vec4 p4(cosf(theta1) * radius2, sinf(theta1) * radius2, sinf(phi2) * r, 1);

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
 * ベースの中心 p、高さ h、半径 radiusの円柱を描画する。
 */
void drawCylinder(const glm::vec3& p, float h, float r, const glm::vec3& color) {
	int slices = 12;

	glColor3f(color.r, color.g, color.b);
	glBegin(GL_QUADS);
	for (int i = 0; i < slices; ++i) {
		float theta1 = M_PI * 2.0 * (float)i / slices;
		float theta2 = M_PI * 2.0 * (float)(i + 1) / slices;

		glNormal3f(cosf(theta1), sinf(theta1), 0);
		glVertex3f(cosf(theta1) * r, sinf(theta1) * r, p.z);
		glNormal3f(cosf(theta2), sinf(theta2), 0);
		glVertex3f(cosf(theta2) * r, sinf(theta2) * r, p.z);
		glNormal3f(cosf(theta2), sinf(theta2), 0);
		glVertex3f(cosf(theta2) * r, sinf(theta2) * r, p.z + h);
		glNormal3f(cosf(theta1), sinf(theta1), 0);
		glVertex3f(cosf(theta1) * r, sinf(theta1) * r, p.z + h);
	}
	glEnd();
}

void drawCylinder(const glm::vec3& p, float h, float r, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	int slices = 12;

	for (int i = 0; i < slices; ++i) {
		float theta1 = M_PI * 2.0 * (float)i / slices;
		float theta2 = M_PI * 2.0 * (float)(i + 1) / slices;

		glm::vec4 p1(cosf(theta1) * r + p.x, sinf(theta1) * r + p.y, p.z, 1);
		glm::vec4 p2(cosf(theta2) * r + p.x, sinf(theta2) * r + p.y, p.z, 1);
		glm::vec4 p3(cosf(theta2) * r + p.x, sinf(theta2) * r + p.y, p.z + h, 1);
		glm::vec4 p4(cosf(theta1) * r + p.x, sinf(theta1) * r + p.y, p.z + h, 1);
		glm::vec4 n1(cosf(theta1), sinf(theta1), 0, 0);
		glm::vec4 n2(cosf(theta2), sinf(theta2), 0, 0);

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
