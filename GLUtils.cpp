﻿#include "GLUtils.h"
#include <QGLWidget>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/random_polygon_2.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <cassert>
#include <list>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Partition_traits_2<K>                         Traits;
typedef Traits::Point_2                                     Point_2;
typedef Traits::Polygon_2                                   Polygon_2;
typedef Polygon_2::Vertex_iterator                          Vertex_iterator;
typedef std::list<Polygon_2>                                Polygon_list;
typedef CGAL::Creator_uniform_2<int, Point_2>               Creator;
typedef CGAL::Random_points_in_square_2< Point_2, Creator > Point_generator;

//typedef boost::geometry::model::polygon<point_type>			polygon_type;
typedef boost::geometry::model::d2::point_xy<double>		point_2d;

#ifndef M_PI
#define M_PI	3.14159265359
#endif

#ifndef SQR
#define SQR(x)	((x) * (x))
#endif

namespace glutils {

/**
 * Test if the point is inside the polygon
 */
bool isWithinPolygon(const glm::vec2& p, const std::vector<glm::vec2>& points) {
	boost::geometry::model::ring<point_2d> contour;
	for (int i = 0; i < points.size(); ++i) {
		contour.push_back(point_2d(points[i].x, points[i].y));
	}
	boost::geometry::correct(contour);

	return boost::geometry::within(point_2d(p.x, p.y), contour);
}

/*
 * Return the distance from segment ab to point c.
 */
float distance(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, bool segmentOnly) {
	float r_numerator = (c.x-a.x) * (b.x-a.x) + (c.y-a.y) * (b.y-a.y);
	float r_denomenator = (b.x-a.x) * (b.x-a.x) + (b.y-a.y) * (b.y-a.y);

	if (r_denomenator <= 0.0f) {
		return (a - c).length();
	}

	float r = r_numerator / r_denomenator;

	if (segmentOnly && (r < 0 || r > 1)) {
		float dist1 = SQR(c.x - a.x) + SQR(c.y - a.y);
		float dist2 = SQR(c.x - b.x) + SQR(c.y - b.y);
		if (dist1 < dist2) {	
			return sqrt(dist1);
		} else {
			return sqrt(dist2);
		}
	} else {
		return abs((a.y-c.y) * (b.x-a.x) - (a.x-c.x) * (b.y-a.y)) / sqrt(r_denomenator);
	}
}

/*
 * Return the distance from segment ab to point c.
 */
float distance(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	glm::vec3 v = glm::normalize(b - a);
	return glm::length(glm::cross(a - c, v));
}

/**
 * Line-line intersection
 * Compute the intersection of a line that pass though p1 and whose vector is v1 and another line that pass through p2 and whose vector is v2.
 */
glm::vec3 lineLineIntersection(const glm::vec3& p1, const glm::vec3& v1, const glm::vec3& p2, const glm::vec3& v2) {
	// tentative implementation (might be wrong)
	glm::mat2 m1;
	m1[0].x = glm::dot(v1, v1);
	m1[0].y = glm::dot(v1, v2);
	m1[1].x = glm::dot(v1, -v2);
	m1[1].y = glm::dot(v2, -v2);

	glm::vec2 m2;
	m2.x = glm::dot(v1, p2 - p1);
	m2.y = glm::dot(v2, p2 - p1);

	glm::vec2 st = glm::inverse(m1) * m2;

	glm::vec3 pp1 = p1 + v1 * st.x;
	glm::vec3 pp2 = p2 + v2 * st.y;

	return (pp1 + pp2) * 0.5f;
}

/**
 * Ray-Plane intersection
 * Compute the intersection of a ray that starts from a and its direction v, and a plane whose normal is n and p is on the plane.
 */
glm::vec3 rayPlaneIntersection(const glm::vec3& a, const glm::vec3& v, const glm::vec3& p, const glm::vec3& n) {
	return a + v * glm::dot(p - a, n) / glm::dot(v, n);
}

/**
 * Ray-Triangle intersection
 */
bool rayTriangleIntersection(const glm::vec3& a, const glm::vec3& v, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, glm::vec3& intPt) {
	glm::vec3 n = glm::cross(p2 - p1, p3 - p1);
	intPt = a + v * glm::dot(p1 - a, n) / glm::dot(v, n);

	cv::Mat_<double> L(3, 1);
	L(0, 0) = (intPt - p1).x;
	L(1, 0) = (intPt - p1).y;
	L(2, 0) = (intPt - p1).z;
	cv::Mat_<double> R(3, 2);
	R(0, 0) = (p2 - p1).x;
	R(1, 0) = (p2 - p1).y;
	R(2, 0) = (p2 - p1).z;
	R(0, 1) = (p3 - p1).x;
	R(1, 1) = (p3 - p1).y;
	R(2, 1) = (p3 - p1).z;

	cv::Mat_<double> st = R.inv(cv::DECOMP_SVD) * L;
	
	if (st(0, 0) >= 0 && st(1, 0) >= 0 && st(0, 0) + st(1, 0) <= 1) return true;
	else return false;
}

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

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), color, glm::vec2(0, 0)));
	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n), color, glm::vec2(1, 0)));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), color, glm::vec2(1, 1)));

	vertices.push_back(Vertex(glm::vec3(p1), glm::vec3(n), color, glm::vec2(0, 0)));
	vertices.push_back(Vertex(glm::vec3(p3), glm::vec3(n), color, glm::vec2(1, 1)));
	vertices.push_back(Vertex(glm::vec3(p4), glm::vec3(n), color, glm::vec2(0, 1)));
}

void drawQuad(float w, float h, const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, const glm::vec2& t4, const glm::mat4& mat, std::vector<Vertex>& vertices) {
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

void drawPolygon(const std::vector<glm::vec3>& points, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	glm::vec4 p1(points.back(), 1);
	p1 = mat * p1;
	glm::vec4 p2(points[0], 1);
	p2 = mat * p2;

	glm::vec3 normal;
	bool normal_computed = false;

	for (int i = 0; i < points.size() - 2; ++i) {
		glm::vec4 p3(points[i + 1], 1);
		p3 = mat * p3;

		if (!normal_computed) {
			normal = glm::cross(glm::vec3(p2 - p1), glm::vec3(p3 - p1));
			normal = glm::vec3(mat * glm::vec4(normal, 0));
			normal_computed = true;
		}

		vertices.push_back(Vertex(glm::vec3(p1), normal, color));
		vertices.push_back(Vertex(glm::vec3(p2), normal, color));
		vertices.push_back(Vertex(glm::vec3(p3), normal, color));

		p2 = p3;
	}
}

void drawPolygon(const std::vector<glm::vec2>& points, const glm::vec3& color, const std::vector<glm::vec2>& texCoords, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	glm::vec4 p1(points.back(), 0, 1);
	p1 = mat * p1;
	glm::vec2 t1 = texCoords.back();
	glm::vec4 p2(points[0], 0, 1);
	p2 = mat * p2;
	glm::vec2 t2 = texCoords[0];

	glm::vec3 normal;
	bool normal_computed = false;

	for (int i = 0; i < points.size() - 2; ++i) {
		glm::vec4 p3(points[i + 1], 0, 1);
		p3 = mat * p3;
		glm::vec2 t3 = texCoords[i + 1];

		if (!normal_computed) {
			normal = glm::cross(glm::vec3(p2 - p1), glm::vec3(p3 - p1));
			normal = glm::vec3(mat * glm::vec4(normal, 0));
			normal_computed = true;
		}

		vertices.push_back(Vertex(glm::vec3(p1), normal, color, t1));
		vertices.push_back(Vertex(glm::vec3(p2), normal, color, t2));
		vertices.push_back(Vertex(glm::vec3(p3), normal, color, t3));

		p2 = p3;
		t2 = t3;
	}
}

void drawConcavePolygon(const std::vector<glm::vec2>& points, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	float max_x = 0.0f;
	float max_y = 0.0f;

	Polygon_2 polygon;
	for (int i = 0; i < points.size(); ++i) {
		polygon.push_back(Point_2(points[i].x, points[i].y));

		if (points[i].x > max_x) {
			max_x = points[i].x;
		}
		if (points[i].y > max_y) {
			max_y = points[i].y;
		}
	}
	
	// tesselate the concave polygon
	Polygon_list partition_polys;
	Traits       partition_traits;
	CGAL::greene_approx_convex_partition_2(polygon.vertices_begin(), polygon.vertices_end(), std::back_inserter(partition_polys), partition_traits);

	for (auto fit = partition_polys.begin(); fit != partition_polys.end(); ++fit) {
		std::vector<glm::vec2> pts;
		std::vector<glm::vec2> texCoords;
		for (auto vit = fit->vertices_begin(); vit != fit->vertices_end(); ++vit) {
			pts.push_back(glm::vec2(vit->x(), vit->y()));
			texCoords.push_back(glm::vec2(vit->x() / max_x, vit->y() / max_y));
		}

		drawPolygon(pts, color, texCoords, mat, vertices);
	}
}

void drawGrid(float width, float height, float cell_size, const glm::vec3& lineColor, const glm::vec3& backgroundColor, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	drawQuad(width, height, backgroundColor, mat, vertices);
	
	for (float x = 0; x < width * 0.5; x += cell_size) {
		glm::mat4 m = glm::translate(mat, glm::vec3(x, -height * 0.5, 0));
		drawCylinderY(0.1, 0.1, height, lineColor, m, vertices);
		if (x > 0) {
			glm::mat4 m = glm::translate(mat, glm::vec3(-x, -height * 0.5, 0));
			drawCylinderY(0.1, 0.1, height, lineColor, m, vertices);
		}
	}

	for (float y = 0; y < height * 0.5; y += cell_size) {
		glm::mat4 m = glm::translate(mat, glm::vec3(-width * 0.5, y, 0));
		drawCylinderX(0.1, 0.1, width, lineColor, m, vertices);
		if (y > 0) {
			glm::mat4 m = glm::translate(mat, glm::vec3(-width * 0.5, -y, 0));
			drawCylinderX(0.1, 0.1, width, lineColor, m, vertices);
		}
	}
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
	vertices.push_back(Vertex(glm::vec3(p2), glm::vec3(n3), color));
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

void drawEllipsoid(float r1, float r2, float r3, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	int slices = 32;
	int stacks = 16;

	for (int i = 0; i < stacks; ++i) {
		float phi1 = M_PI * (float)i / stacks - M_PI * 0.5;
		float phi2 = M_PI * (float)(i + 1) / stacks - M_PI * 0.5;

		for (int j = 0; j < slices; ++j) {
			float theta1 = M_PI * 2.0 * (float)j / slices;
			float theta2 = M_PI * 2.0 * (float)(j + 1) / slices;

			/*
			float x_base1 = cosf(theta1) * r1;
			float y_base1 = sinf(theta1) * r2;
			float r_base1 = sqrt(x_base1 * x_base1 + y_base1 * y_base1);

			float x_base2 = cosf(theta2) * r1;
			float y_base2 = sinf(theta2) * r2;
			float r_base2 = sqrt(x_base2 * x_base2 + y_base2 * y_base2);
			*/

			glm::vec4 p1(cosf(phi1) * cosf(theta1) * r1, cosf(phi1) * sinf(theta1) * r2, sinf(phi1) * r3, 1);
			glm::vec4 p2(cosf(phi1) * cosf(theta2) * r1, cosf(phi1) * sinf(theta2) * r2, sinf(phi1) * r3, 1);
			glm::vec4 p3(cosf(phi2) * cosf(theta2) * r1, cosf(phi2) * sinf(theta2) * r2, sinf(phi2) * r3, 1);
			glm::vec4 p4(cosf(phi2) * cosf(theta1) * r1, cosf(phi2) * sinf(theta1) * r2, sinf(phi2) * r3, 1);

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
 * X軸方向に高さ h、底面の半径 r1、上面の半径 r2の円錐を描画する。
 */
void drawCylinderX(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices) {
	float phi = atan2(radius1 - radius2, h);

	for (int i = 0; i < slices; ++i) {
		float theta1 = M_PI * 2.0 * (float)i / slices;
		float theta2 = M_PI * 2.0 * (float)(i + 1) / slices;

		glm::vec4 p1(0, cosf(theta1) * radius1, sinf(theta1) * radius1, 1);
		glm::vec4 p2(0, cosf(theta2) * radius1, sinf(theta2) * radius1, 1);
		glm::vec4 p3(h, cosf(theta2) * radius2, sinf(theta2) * radius2, 1);
		glm::vec4 p4(h, cosf(theta1) * radius2, sinf(theta1) * radius2, 1);
		glm::vec4 n1(sinf(phi), cosf(theta1) * cosf(phi), sinf(theta1) * cosf(phi), 0);
		glm::vec4 n2(sinf(phi), cosf(theta2) * cosf(phi), sinf(theta2) * cosf(phi), 0);

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
 * Y軸方向に高さ h、底面の半径 r1、上面の半径 r2の円錐を描画する。
 */
void drawCylinderY(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices) {
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
void drawCylinderZ(float radius1, float radius2, float h, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices, int slices) {
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
	// X軸を描画（赤色）
	glm::mat4 m1 = glm::rotate(mat, deg2rad(90), glm::vec3(0, 1, 0));
	drawArrow(radius, length, glm::vec3(1, 0, 0), m1, vertices);

	// Y軸を描画（緑色）
	glm::mat4 m2 = glm::rotate(mat, deg2rad(-90), glm::vec3(1, 0, 0));
	drawArrow(radius, length, glm::vec3(0, 1, 0), m2, vertices);

	// Z軸を描画 (青色）
	drawArrow(radius, length, glm::vec3(0, 0, 1), mat, vertices);
}

void drawTube(std::vector<glm::vec3>& points, float radius, const glm::vec3& color, std::vector<Vertex>& vertices, int slices) {
	if (points.size() <= 1) return;

	glm::mat4 modelMat1, modelMat2;
	glm::vec3 x_dir, y_dir, z_dir, x_dir2, y_dir2, z_dir2;
	std::vector<glm::vec3> circle_points(slices);
	std::vector<glm::vec3> circle_normals(slices);

	{
		// 最初の円筒形の、ローカル座標系を計算
		modelMat1 = glm::translate(modelMat1, glm::vec3(points[0]));
		y_dir = glm::normalize(points[1] - points[0]);
		z_dir = glm::normalize(glm::cross(points[2] - points[1], y_dir));
		x_dir = glm::normalize(glm::cross(y_dir, z_dir));
		z_dir = glm::normalize(glm::cross(x_dir, y_dir));

		// ローカル座標系への変換行列を計算		
		modelMat1[0].x = x_dir.x; modelMat1[0].y = x_dir.y; modelMat1[0].z = x_dir.z;
		modelMat1[1].x = y_dir.x; modelMat1[1].y = y_dir.y; modelMat1[1].z = y_dir.z;
		modelMat1[2].x = z_dir.x; modelMat1[2].y = z_dir.y; modelMat1[2].z = z_dir.z;
	}

	// 円周の頂点座標を計算
	for (int k = 0; k < slices; ++k) {
		float theta = (float)k / slices * M_PI * 2.0f;

		glm::vec4 p(cosf(theta) * radius, 0, -sinf(theta) * radius, 1);
		glm::vec4 n(cosf(theta), 0, -sinf(theta), 0);
		p = modelMat1 * p;
		n = modelMat1 * n;
		circle_points[k] = glm::vec3(p);
		circle_normals[k] = glm::vec3(n);
	}

	for (int i = 0; i < points.size() - 1; ++i) {
		modelMat2 = glm::translate(glm::mat4(), glm::vec3(points[i + 1]));

		if (i < points.size() - 2) {
			// この円筒形の、ローカル座標系を計算
			y_dir2 = glm::normalize(points[i + 2] - points[i + 1]);
			if (i < points.size() - 3) {
				z_dir2 = glm::normalize(glm::cross(y_dir2, points[i + 1] - points[i]));
				if (glm::dot(z_dir, z_dir2) < 0) {
					z_dir2 = -z_dir2;
				}
			} else {
				z_dir2 = z_dir;
			}
			x_dir2 = glm::normalize(glm::cross(y_dir2, z_dir2));
			z_dir2 = glm::normalize(glm::cross(x_dir2, y_dir2));

			// ローカル座標系への変換行列を計算		
			modelMat2[0].x = x_dir2.x; modelMat2[0].y = x_dir2.y; modelMat2[0].z = x_dir2.z;
			modelMat2[1].x = y_dir2.x; modelMat2[1].y = y_dir2.y; modelMat2[1].z = y_dir2.z;
			modelMat2[2].x = z_dir2.x; modelMat2[2].y = z_dir2.y; modelMat2[2].z = z_dir2.z;
		} else {
			modelMat2 = glm::translate(glm::mat4(), points[i + 1] - points[i]) * modelMat1;
			//modelMat2 = glm::translate(modelMat1, points[i + 1] - points[i]);
		}


		float h = glm::length(points[i + 1] - points[i]);
		std::vector<glm::vec3> circle_points2(slices);
		std::vector<glm::vec3> circle_normals2(slices);

		for (int k = 0; k < slices; ++k) {
			float theta = (float)k / slices * M_PI * 2.0f;

			glm::vec4 p(cosf(theta) * radius, 0, -sinf(theta) * radius, 1);

			glm::vec3 p1 = glm::vec3(modelMat1 * p) + points[i + 1] - points[i];
			glm::vec3 p2 = glm::vec3(modelMat2 * p);
			glm::vec3 pp = (p1 + p2) * 0.5f;
			
			circle_normals2[k] = glm::normalize(pp - points[i + 1]);
			circle_points2[k] = circle_normals2[k] * radius + points[i + 1];
		}

		for (int k = 0; k < slices; ++k) {
			vertices.push_back(Vertex(glm::vec3(circle_points[k]), glm::vec3(circle_normals[k]), color));
			vertices.push_back(Vertex(glm::vec3(circle_points[(k+1)%slices]), glm::vec3(circle_normals[(k+1)%slices]), color));
			vertices.push_back(Vertex(glm::vec3(circle_points2[(k+1)%slices]), glm::vec3(circle_normals2[(k+1)%slices]), color));

			vertices.push_back(Vertex(glm::vec3(circle_points[k]), glm::vec3(circle_normals[k]), color));
			vertices.push_back(Vertex(glm::vec3(circle_points2[(k+1)%slices]), glm::vec3(circle_normals2[(k+1)%slices]), color));
			vertices.push_back(Vertex(glm::vec3(circle_points2[k]), glm::vec3(circle_normals2[k]), color));
		}


		modelMat1 = modelMat2;
		circle_points = circle_points2;
		circle_normals = circle_normals2;
		x_dir = x_dir2; y_dir = y_dir2; z_dir = z_dir2;
	}
}

void drawCurvilinearMesh(int numX, int numY, std::vector<glm::vec3>& points, const glm::vec3& color, const glm::mat4& mat, std::vector<Vertex>& vertices) {
	for (int i = 0; i < numY - 1; ++i) {
		for (int j = 0; j < numX - 1; ++j) {
			glm::vec3 p1 = glm::vec3(mat * glm::vec4(points[i * numX + j], 1));
			glm::vec3 p2 = glm::vec3(mat * glm::vec4(points[i * numX + j + 1], 1));
			glm::vec3 p3 = glm::vec3(mat * glm::vec4(points[(i + 1) * numX + j + 1], 1));
			glm::vec3 p4 = glm::vec3(mat * glm::vec4(points[(i + 1) * numX + j], 1));

			glm::vec3 normal = glm::cross(p2 - p1, p3 - p1);

			vertices.push_back(Vertex(p1, normal, color));
			vertices.push_back(Vertex(p2, normal, color));
			vertices.push_back(Vertex(p3, normal, color));

			vertices.push_back(Vertex(p1, normal, color));
			vertices.push_back(Vertex(p3, normal, color));
			vertices.push_back(Vertex(p4, normal, color));
		}
	}
}

float deg2rad(float degree) {
	return degree * M_PI / 180.0;
}

}
