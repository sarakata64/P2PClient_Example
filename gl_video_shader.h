/**
 * This file is part of janus_client project.
 * Author:    Jackie Ou
 * Created:   2020-10-01
 **/

#pragma once

#include <memory>
#include "absl/types/optional.h"
#include "api/video/video_rotation.h"

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
typedef char GLchar;

class QOpenGLFunctions_3_3_Core;

class GLVideoShader 
	: public std::enable_shared_from_this<GLVideoShader>
{
public:
        GLVideoShader(QOpenGLFunctions_3_3_Core* arg_F);

	~GLVideoShader();

	bool createAndSetupI420Program();

	bool createAndSetupNV12Program();

	bool prepareVertexBuffer(webrtc::VideoRotation rotation);

	void applyShadingForFrame(int width,
		int height,
		webrtc::VideoRotation rotation,
		GLuint yPlane,
		GLuint uPlane,
		GLuint vPlane);

	void applyShadingForFrame(int width,
		int height,
		webrtc::VideoRotation rotation,
		GLuint yPlane,
		GLuint uvPlane);

protected:
	GLuint createShader(GLenum type, const GLchar* source);

	GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

	GLuint createProgramFromFragmentSource(const char fragmentShaderSource[]);

	bool createVertexBuffer(GLuint* vertexBuffer, GLuint* vertexArray);

	void setVertexData(webrtc::VideoRotation rotation);

private:
	GLuint _vertexBuffer = 0;

	GLuint _vertexArray = 0;

	// Store current rotation and only upload new vertex data when rotation changes.
	absl::optional<webrtc::VideoRotation> _currentRotation;

	GLuint _i420Program = 0;

	GLuint _nv12Program = 0;

        // Added
          QOpenGLFunctions_3_3_Core *F;
};

