#pragma once

#include <QString>

class Shader
{
public:
	Shader() {}

	uint createProgram(const char *vertex_source, const char* fragment_source);

private:
	void loadTextFile(const std::string& filename, std::string& str);
	GLuint compileShader(const std::string& source, GLuint mode);

private:
	GLuint program;
	GLuint vertex_shader;
	GLuint fragment_shader;
};

