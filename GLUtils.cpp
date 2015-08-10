﻿#include "GLUtils.h"
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

void drawQuad(float w, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	glm::vec4 p1(-w * 0.5, -h * 0.5, 0, 1);
	glm::vec4 p2(w * 0.5, -h * 0.5, 0, 1);
	glm::vec4 p3(w * 0.5, h * 0.5, 0, 1);
	glm::vec4 p4(-w * 0.5, h * 0.5, 0, 1);
	glm::vec4 n(0, 0, 1, 0);

	p1 = mat * p1;
	p2 = mat * p2;
	p3 = mat * p3;
	p4 = mat * p4;
	n = mat * n;

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), color, glm::vec3(0, 0, 0)));
	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n), color, glm::vec3(1, 0, 0)));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), color, glm::vec3(1, 1, 0)));

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), color, glm::vec3(0, 0, 0)));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), color, glm::vec3(1, 1, 0)));
	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n), color, glm::vec3(0, 1, 0)));
}

void drawQuad(float w, float h, const glm::vec3& t1, const glm::vec3& t2, const glm::vec3& t3, const glm::vec3& t4, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	glm::vec4 p1(-w * 0.5, -h * 0.5, 0, 1);
	glm::vec4 p2(w * 0.5, -h * 0.5, 0, 1);
	glm::vec4 p3(w * 0.5, h * 0.5, 0, 1);
	glm::vec4 p4(-w * 0.5, h * 0.5, 0, 1);
	glm::vec4 n(0, 0, 1, 0);

	p1 = mat * p1;
	p2 = mat * p2;
	p3 = mat * p3;
	p4 = mat * p4;
	n = mat * n;

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), glm::vec3(1, 1, 1), t1));
	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n), glm::vec3(1, 1, 1), t2));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), glm::vec3(1, 1, 1), t3));

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), glm::vec3(1, 1, 1), t1));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), glm::vec3(1, 1, 1), t3));
	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n), glm::vec3(1, 1, 1), t4));
}

void drawBox(float length_x, float length_y, float length_z, glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	glm::vec4 p1(-length_x * 0.5, -length_y * 0.5, -length_z * 0.5, 1);
	glm::vec4 p2(length_x * 0.5, -length_y * 0.5, -length_z * 0.5, 1);
	glm::vec4 p3(length_x * 0.5, length_y * 0.5, -length_z * 0.5, 1);
	glm::vec4 p4(-length_x * 0.5, length_y * 0.5, -length_z * 0.5, 1);
	glm::vec4 p5(-length_x * 0.5, -length_y * 0.5, length_z * 0.5, 1);
	glm::vec4 p6(length_x * 0.5, -length_y * 0.5, length_z * 0.5, 1);
	glm::vec4 p7(length_x * 0.5, length_y * 0.5, length_z * 0.5, 1);
	glm::vec4 p8(-length_x * 0.5, length_y * 0.5, length_z * 0.5, 1);
	glm::vec4 n1(-1, 0, 0, 0);
	glm::vec4 n2(1, 0, 0, 0);
	glm::vec4 n3(0, -1, 0, 0);
	glm::vec4 n4(0, 1, 0, 0);
	glm::vec4 n5(0, 0, -1, 0);
	glm::vec4 n6(0, 0, 1, 0);

	p1 = mat * p1;
	p2 = mat * p2;
	p3 = mat * p3;
	p4 = mat * p4;
	p5 = mat * p5;
	p6 = mat * p6;
	p7 = mat * p7;
	p8 = mat * p8;
	n1 = mat * n1;
	n2 = mat * n2;
	n3 = mat * n3;
	n4 = mat * n4;
	n5 = mat * n5;
	n6 = mat * n6;

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n5), color));
	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n5), color));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n5), color));

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n5), color));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n5), color));
	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n5), color));

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n3), color));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n3), color));
	vertices.push_back(Vertex(glm::vec3(p6), glm::vec3(n3), color));

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n3), color));
	vertices.push_back(Vertex(glm::vec3(p6), glm::vec3(n3), color));
	vertices.push_back(Vertex(glm::vec3(p5), glm::vec3(n3), color));

	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n2), color));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n2), color));
	vertices.push_back(Vertex(glm::vec3(p7), glm::vec3(n2), color));

	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n2), color));
	vertices.push_back(Vertex(glm::vec3(p7), glm::vec3(n2), color));
	vertices.push_back(Vertex(glm::vec3(p6), glm::vec3(n2), color));

	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n4), color));
	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n4), color));
	vertices.push_back(Vertex(glm::vec3(p8), glm::vec3(n4), color));

	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n4), color));
	vertices.push_back(Vertex(glm::vec3(p8), glm::vec3(n4), color));
	vertices.push_back(Vertex(glm::vec3(p7), glm::vec3(n4), color));

	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n1), color));
	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n1), color));
	vertices.push_back(Vertex(glm::vec3(p5), glm::vec3(n1), color));

	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n1), color));
	vertices.push_back(Vertex(glm::vec3(p5), glm::vec3(n1), color));
	vertices.push_back(Vertex(glm::vec3(p8), glm::vec3(n1), color));

	vertices.push_back(Vertex(glm::vec3(p5), glm::vec3(n6), color));
	vertices.push_back(Vertex(glm::vec3(p6), glm::vec3(n6), color));
	vertices.push_back(Vertex(glm::vec3(p7), glm::vec3(n6), color));

	vertices.push_back(Vertex(glm::vec3(p5), glm::vec3(n6), color));
	vertices.push_back(Vertex(glm::vec3(p7), glm::vec3(n6), color));
	vertices.push_back(Vertex(glm::vec3(p8), glm::vec3(n6), color));
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
 * Y軸方向に高さ h、底面の半径 r1、上面の半径 r2の円錐を描画する。
 */
void drawCylinderY(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	int slices = 12;

	float phi = atan2(radius1 - radius2, h);

	for (int i = 0; i < slices; ++i) {
		float theta1 = M_PI * 2.0 * (float)i / slices;
		float theta2 = M_PI * 2.0 * (float)(i + 1) / slices;

		glm::vec4 p1(cosf(theta1) * radius1, 0, sinf(theta1) * radius1, 1);
		glm::vec4 p2(cosf(theta2) * radius1, 0, sinf(theta2) * radius1, 1);
		glm::vec4 p3(cosf(theta2) * radius2, h, sinf(theta2) * radius2, 1);
		glm::vec4 p4(cosf(theta1) * radius2, h, sinf(theta1) * radius2, 1);
		glm::vec4 n1(cosf(theta1) * cosf(phi), sinf(phi), sinf(theta1) * cosf(phi), 0);
		glm::vec4 n2(cosf(theta2) * cosf(phi), sinf(phi), sinf(theta2) * cosf(phi), 0);

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

/**
 * Z軸方向に高さ h、底面の半径 r1、上面の半径 r2の円錐を描画する。
 */
void drawCylinderZ(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
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

/**
 * Z軸方向に、指定された長さ、色、半径の矢印を描画する。
 */
void drawArrow(float radius, float length, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	drawCylinderZ(radius, radius, length - radius * 4, color, mat, vertices);
	glm::mat4 m = glm::translate(mat, glm::vec3(0, 0, length - radius * 4));
	drawCylinderZ(radius * 2, 0, radius * 4, color, m, vertices);
}

void drawAxes(float radius, float length, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	// X軸を描画
	glm::mat4 m1 = glm::rotate(mat, deg2rad(90), glm::vec3(0, 1, 0));
	drawArrow(radius, length, glm::vec3(0, 0, 1), m1, vertices);

	// Y軸を描画
	glm::mat4 m2 = glm::rotate(mat, deg2rad(-90), glm::vec3(1, 0, 0));
	drawArrow(radius, length, glm::vec3(1, 0, 0), m2, vertices);

	// Z軸を描画
	drawArrow(radius, length, glm::vec3(0, 1, 0), mat, vertices);
}

float deg2rad(float degree) {
	return degree * M_PI / 180.0;
}

}
