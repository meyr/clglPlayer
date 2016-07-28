#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "opengl.h"
static char vertex_shader[] = {
	"#version 330 core\n"
	"// Input vertex data, different for all executions of this shader.\n"
	"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
	"layout(location = 1) in vec2 vertexUV;\n"
	"// Output data ; will be interpolated for each fragment.\n"
	"out vec2 UV;\n"
	"void main(){\n"
		"// Output position of the vertex, in clip space : MVP * position\n"
		"gl_Position =  vec4(vertexPosition_modelspace,1);\n"
		"// UV of the vertex. No special space for this one.\n"
		"UV = vertexUV;\n"
	"}\n"
};

static char fragment_shader[] = {
	"#version 330 core\n"
	"// Interpolated values from the vertex shaders\n"
	"in vec2 UV;\n"
	"// Ouput data\n"
	"out vec3 color;\n"
	"// Values that stay constant for the whole mesh.\n"
	"uniform sampler2D myTextureSampler;\n"
	"void main(){\n"
	"	// Output color = color of the texture at the specified UV\n"
	"	color = texture( myTextureSampler, UV ).rgb;\n"
	"}\n"
};

static GLubyte* ReadShader(const char* filename)
{
	int len;
	FILE *infile;
	GLubyte *source;

	myfopen(infile, filename, "rb");
	if (!infile) {
		printf("unable to open file %s\n", filename);
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	source = malloc(len + 1);

	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;

	return source;
}

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{

	GLint Result = GL_FALSE;
	int InfoLogLength;
	char *ErrorMessage;
	char *VertexShaderSource, *FragmentShaderSource;
	GLuint ProgramID, VertexShaderID, FragmentShaderID;

	// Create the shaders
	VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	//VertexShaderSource = ReadShader(vertex_file_path);
	VertexShaderSource = vertex_shader; 

	// Read the Fragment Shader code from the file
	//FragmentShaderSource = ReadShader(fragment_file_path);
	FragmentShaderSource = fragment_shader;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	glShaderSource(VertexShaderID, 1, &VertexShaderSource , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 1) {
		ErrorMessage = malloc(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, ErrorMessage);
		printf("%s\n", ErrorMessage);
		free(ErrorMessage);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	glShaderSource(FragmentShaderID, 1, &FragmentShaderSource , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 1) {
		ErrorMessage = malloc(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, ErrorMessage);
		printf("%s\n", ErrorMessage);
		free(ErrorMessage);
	}



	// Link the program
	printf("Linking program\n");
	ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		ErrorMessage = malloc(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ErrorMessage);
		printf("%s\n", ErrorMessage);
		free(ErrorMessage);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
