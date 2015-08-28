﻿#include "RenderManager.h"
#include <iostream>
#include "Shader.h"
#include <QImage>
#include <QGLWidget>

GeometrySubObject::GeometrySubObject(const QString& name, const std::vector<Vertex>& vertices) : name(name), vertices(vertices) {
}

VaoObject::VaoObject() {
	vaoCreated = false;
	vaoOutdated = true;
	num_vertices = 0;
}

void VaoObject::addSubObject(GeometrySubObject* sub_object) {
	this->sub_objects.push_back(sub_object);
	this->num_vertices += sub_object->vertices.size();
	vaoOutdated = true;
}

/**
 * Create VAO according to the vertices.
 */
void VaoObject::createVAO() {
	// VAOが作成済みで、最新なら、何もしないで終了
	//if (vaoCreated && !vaoOutdated) return;

	if (vaoCreated) {
		// 古いVAO, VBOを解放する
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	// create vao and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create VBO and tranfer the vertices data to GPU buffer
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * num_vertices, NULL, GL_STATIC_DRAW);
	int offset = 0;
	for (int i = 0; i < sub_objects.size(); ++i) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Vertex) * sub_objects[i]->vertices.size(), sub_objects[i]->vertices.data());
		offset += sizeof(Vertex) * sub_objects[i]->vertices.size();
	}
	
	// configure the attributes in the vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	// unbind the vao
	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VAOを作成済み、最新とマークする
	vaoCreated = true;
	vaoOutdated = false;
}

RenderManager::RenderManager() {
}

void RenderManager::init(const std::string& vertex_file, const std::string& geometry_file, const std::string& fragment_file, int shadowMapSize) {
	// init glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}

	// load shaders
	Shader shader;
	if (geometry_file.empty()) {
		program = shader.createProgram(vertex_file, fragment_file);
	} else {
		program = shader.createProgram(vertex_file, geometry_file, fragment_file);
	}
	glUseProgram(program);

	// ダミーのtexture idを作成する。
	// これにより、実際に使われるtexture idは1以上の値となる
	GLuint texId;
	glGenTextures(1, &texId);

	shadow.init(program, shadowMapSize, shadowMapSize);
}

void RenderManager::addObject(const QString& object_name, const QString& texture_file, const std::vector<Vertex>& vertices) {
	GLuint texId;
	
	if (texture_file.length() > 0) {
		// テクスチャファイルがまだ読み込まれていない場合は、ロードする
		if (!textures.contains(texture_file)) {
			texId = loadTexture(texture_file);
			textures[texture_file] = texId;
		} else {
			texId = textures[texture_file];
		}
	} else {
		texId = 0;
	}

	GeometrySubObject* gso = new GeometrySubObject(object_name, vertices);
	vao_objects[texId].addSubObject(gso);
	vao_objects[texId].vaoOutdated = true;
	name_objects[object_name].sub_objects.push_back(gso);

	/*
	if (objects.contains(object_name)) {
		if (objects[object_name].contains(texId)) {
			objects[object_name][texId].addVertices(vertices);
		} else {
			objects[object_name][texId] = GeometryObject(vertices);
		}
	} else {
		objects[object_name][texId] = GeometryObject(vertices);
	}
	*/
}

void RenderManager::removeObject(const QString& object_name) {
	name_objects.remove(object_name);
	for (auto it = vao_objects.begin(); it != vao_objects.end(); ++it) {
		bool removed = false;
		
		for (int i = 0; i < it->sub_objects.size(); ) {
			if (it->sub_objects[i]->name == object_name) {
				it->sub_objects.erase(it->sub_objects.begin() + i);
				removed = true;
			} else {
				++i;
			}
		}

		if (removed) {
			it->vaoOutdated = true;
		}
	}

	/*
	// VAO、VBOを解放する
	for (auto it = objects[object_name].begin(); it != objects[object_name].end(); ++it) {
		glDeleteBuffers(1, &it->vbo);
		glDeleteVertexArrays(1, &it->vao);
	}
	*/
}

void RenderManager::renderAll(bool wireframe) {
	for (auto it = vao_objects.begin(); it != vao_objects.end(); ++it) {
		GLuint texId = it.key();
		
		// vaoを作成
		it->createVAO();

		if (texId > 0) {
			// テクスチャなら、バインドする
			glBindTexture(GL_TEXTURE_2D, texId);
			glUniform1i(glGetUniformLocation(program, "textureEnabled"), 1);
			glUniform1i(glGetUniformLocation(program, "tex0"), 0);
		} else {
			glUniform1i(glGetUniformLocation(program, "textureEnabled"), 0);
		}

		if (wireframe) {
			glUniform1i(glGetUniformLocation(program, "wireframeEnalbed"), 1);
		} else {
			glUniform1i(glGetUniformLocation(program, "wireframeEnalbed"), 0);
		}

		// 描画
		glBindVertexArray(it->vao);
		glDrawArrays(GL_TRIANGLES, 0, it->num_vertices);

		glBindVertexArray(0);

	}
}

/*void RenderManager::render(const QString& object_name, bool wireframe) {
	for (auto it = objects[object_name].begin(); it != objects[object_name].end(); ++it) {
		GLuint texId = it.key();
		
		// vaoを作成
		it->createVAO();

		if (texId > 0) {
			// テクスチャなら、バインドする
			glBindTexture(GL_TEXTURE_2D, texId);
			glUniform1i(glGetUniformLocation(program, "textureEnabled"), 1);
			glUniform1i(glGetUniformLocation(program, "tex0"), 0);
		} else {
			glUniform1i(glGetUniformLocation(program, "textureEnabled"), 0);
		}

		if (wireframe) {
			glUniform1i(glGetUniformLocation(program, "wireframeEnalbed"), 1);
		} else {
			glUniform1i(glGetUniformLocation(program, "wireframeEnalbed"), 0);
		}

		// 描画
		glBindVertexArray(it->vao);
		glDrawArrays(GL_TRIANGLES, 0, it->vertices.size());

		glBindVertexArray(0);
	}
}
*/

void RenderManager::updateShadowMap(GLWidget3D* glWidget3D, const glm::vec3& light_dir, const glm::mat4& light_mvpMatrix) {
	shadow.update(glWidget3D, light_dir, light_mvpMatrix);
}

std::vector<GeometryObject*> RenderManager::intersectObjects(const glm::vec2& p, const glm::mat4& mvpMatrix) {
	float min_z = (std::numeric_limits<float>::max)();
	QString intersectedObject;
	uint intersectedVao;
	bool intersected = false;

	for (auto it = vao_objects.begin(); it != vao_objects.end(); ++it) {
		for (int i = 0; i < it->sub_objects.size(); ++i) {
			for (int vi = 0; vi < it->sub_objects[i]->vertices.size(); vi += 3) {
				glm::vec4 p1 = glm::vec4(it->sub_objects[i]->vertices[vi + 0].position, 1);
				glm::vec4 p2 = glm::vec4(it->sub_objects[i]->vertices[vi + 1].position, 1);
				glm::vec4 p3 = glm::vec4(it->sub_objects[i]->vertices[vi + 2].position, 1);
				p1 = mvpMatrix * p1;
				p2 = mvpMatrix * p2;
				p3 = mvpMatrix * p3;

				glm::vec3 intPt;
				if (withinTriangle(p, p1, p2, p3, intPt)) {
					if (intPt.z < min_z && intPt.z > 0) {
						intersected = true;
						min_z = intPt.z;
						intersectedObject = it->sub_objects[i]->name;
						intersectedVao = it.key();
					}
				}

				it->sub_objects[i]->vertices[vi + 0].color = glm::vec3(1, 1, 1);
				it->sub_objects[i]->vertices[vi + 1].color = glm::vec3(1, 1, 1);
				it->sub_objects[i]->vertices[vi + 2].color = glm::vec3(1, 1, 1);
			}
		}
	}

	std::vector<GeometryObject*> ret;

	if (intersected) {
		ret.push_back(&name_objects[intersectedObject]);

		// 選択されたオブジェクトを含むvaoをoutdatedにする
		vao_objects[intersectedVao].vaoOutdated = true;

		for (auto it = name_objects.begin(); it != name_objects.end(); ++it) {
			if (it.key() == intersectedObject) {
				for (int i = 0; i < name_objects[it.key()].sub_objects.size(); ++i) {
					for (int vi = 0; vi < name_objects[it.key()].sub_objects[i]->vertices.size(); ++vi) {
						name_objects[it.key()].sub_objects[i]->vertices[vi].color = glm::vec3(1, 0, 0);
					}
				}
			}
		}
	}

	return ret;
}


GLuint RenderManager::loadTexture(const QString& filename) {
	QImage img;
	if (!img.load(filename)) {
		printf("ERROR: loading %s\n",filename.toUtf8().data());
		return INT_MAX;
	}

	QImage GL_formatted_image;
	GL_formatted_image = QGLWidget::convertToGLFormat(img);
	if (GL_formatted_image.isNull()) {
		printf("ERROR: GL_formatted_image\n");
		return INT_MAX;
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GL_formatted_image.width(), GL_formatted_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits());
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

bool RenderManager::withinTriangle(const glm::vec2& p, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, glm::vec3& intPt) {
	if (a.z < 0 || b.z < 0 || c.z < 0) return false;

	// Compute vectors        
	glm::vec2 v0 = glm::vec2(c / c.w - a / a.w);
	glm::vec2 v1 = glm::vec2(b / b.w - a / a.w);
	glm::vec2 v2 = p - glm::vec2(a / a.w);

	// Compute dot products
	float dot00 = glm::dot(v0, v0);
	float dot01 = glm::dot(v0, v1);
	float dot02 = glm::dot(v0, v2);
	float dot11 = glm::dot(v1, v1);
	float dot12 = glm::dot(v1, v2);

	// Compute barycentric coordinates
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	intPt = glm::vec3(p, a.z + (c.z - a.z) * u + (b.z - a.z) * v);

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}