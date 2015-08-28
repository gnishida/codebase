#pragma once

#include "glew.h"
#include <vector>
#include <QMap>
#include "Vertex.h"
#include "ShadowMapping.h"

class GeometrySubObject {
public:
	QString name;
	std::vector<Vertex> vertices;

public:
	GeometrySubObject();
	GeometrySubObject(const QString& name, const std::vector<Vertex>& vertices);
};

class GeometryObject {
public:
	std::vector<GeometrySubObject*> sub_objects;
};

class VaoObject {
public:
	GLuint vao;
	GLuint vbo;
	std::vector<GeometrySubObject*> sub_objects;
	int num_vertices;
	bool vaoCreated;
	bool vaoOutdated;

public:
	VaoObject();
	void addSubObject(GeometrySubObject* sub_object);
	void createVAO();
};

class RenderManager {
public:
	GLuint program;
	QMap<GLuint, VaoObject> vao_objects;
	QMap<QString, GeometryObject > name_objects;
	QMap<QString, GLuint> textures;
	ShadowMapping shadow;

public:
	RenderManager();

	void init(const std::string& vertex_file, const std::string& geometry_file, const std::string& fragment_file, int shadowMapSize);
	void addObject(const QString& object_name, const QString& texture_file, const std::vector<Vertex>& vertices);
	void removeObject(const QString& object_name);
	void renderAll(bool wireframe = false);
	//void render(const QString& object_name, bool wireframe = false);
	void updateShadowMap(GLWidget3D* glWidget3D, const glm::vec3& light_dir, const glm::mat4& light_mvpMatrix);
	std::vector<QString> intersectObjects(const glm::vec2& p, const glm::mat4& mvpMatrix);

private:
	GLuint loadTexture(const QString& filename);
	bool withinTriangle(const glm::vec2& p, const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, glm::vec3& intPt);
};

