
#include "gl_video_shader.h"
#include <algorithm>
#include <array>
#include <memory>

#include <QOpenGLFunctions_3_3_Core>

#include <qt_windows.h>
#include <gl/GL.h>
#include <QDebug>
#include <QFile>

static const int kYTextureUnit = 0;
static const int kUTextureUnit = 1;
static const int kVTextureUnit = 2;
static const int kUvTextureUnit = 1;

// shaders variables
QByteArray kNV12FragmentShaderSource;
QByteArray kI420FragmentShaderSource;
QByteArray kRTCVertexShaderSource;

GLVideoShader::GLVideoShader(QOpenGLFunctions_3_3_Core *arg_F) : F(arg_F) {

  auto readSource = [](QByteArray &target, const QString &source) {
    QFile sourceFile(source);
    if (sourceFile.open(QIODevice::ReadOnly)) {
      target = sourceFile.readAll();
      sourceFile.close();
    }
  };

  readSource(kRTCVertexShaderSource, ":/shaders/vertex.vert");

  readSource(kI420FragmentShaderSource,
             ":/shaders/I420FragmentShaderSource.frag");

  readSource(kNV12FragmentShaderSource,
             ":/shaders/nv12FragmentShaderSource.frag");

}

GLVideoShader::~GLVideoShader()
{
  F->glDeleteProgram(_i420Program);
        F->glDeleteProgram(_nv12Program);
        F->glDeleteBuffers(1, &_vertexBuffer);
        F->glDeleteVertexArrays(1, &_vertexArray);
}

// Compiles a shader of the given |type| with GLSL source |source| and returns
// the shader handle or 0 on error.
GLuint GLVideoShader::createShader(GLenum type, const GLchar *source) {
        GLuint shader =F-> glCreateShader(type);
	if (!shader) {
		return 0;
	}
        F->glShaderSource(shader, 1, &source, NULL);
        F->glCompileShader(shader);
	GLint compileStatus = GL_FALSE;
        F->glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		GLint logLength = 0;
		// The null termination character is included in the returned log length.
                F->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			std::unique_ptr<char[]> compileLog(new char[logLength]);
			// The returned string is null terminated.
                        F->glGetShaderInfoLog(shader, logLength, NULL, compileLog.get());
                        qDebug()<<"Shader compile error: {}  "<< compileLog.get();
		}
                F->glDeleteShader(shader);
		shader = 0;
	}
	return shader;
}

// Links a shader program with the given vertex and fragment shaders and
// returns the program handle or 0 on error.
GLuint GLVideoShader::createProgram(GLuint vertexShader, GLuint fragmentShader) {
	if (vertexShader == 0 || fragmentShader == 0) {
		return 0;
	}
        GLuint program = F->glCreateProgram();
	if (!program) {
		return 0;
	}
        F->glAttachShader(program, vertexShader);
        F->glAttachShader(program, fragmentShader);
        F->glLinkProgram(program);
	GLint linkStatus = GL_FALSE;
        F->glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
                F->glDeleteProgram(program);
		program = 0;
	}
	return program;
}

// Creates and links a shader program with the given fragment shader source and
// a plain vertex shader. Returns the program handle or 0 on error.
GLuint GLVideoShader::createProgramFromFragmentSource(const char fragmentShaderSource[]) {
	GLuint vertexShader = createShader(GL_VERTEX_SHADER, kRTCVertexShaderSource);
	//RTC_CHECK(vertexShader) << "failed to create vertex shader";
	if (vertexShader == 0) {
                qDebug("failed to create vertex shader");
	}
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	//RTC_CHECK(fragmentShader) << "failed to create fragment shader"; 
	if (fragmentShader == 0) {
                qDebug("failed to create fragment shader");
	}
	GLuint program = createProgram(vertexShader, fragmentShader);
	// Shaders are created only to generate program.
	if (vertexShader) {
                F->glDeleteShader(vertexShader);
	}
	if (fragmentShader) {
                F->glDeleteShader(fragmentShader);
	}

	// Set vertex shader variables 'position' and 'texcoord' in program.
        GLint position =F-> glGetAttribLocation(program, "position");
        GLint texcoord =F-> glGetAttribLocation(program, "texcoord");
	if (position < 0 || texcoord < 0) {
                F->glDeleteProgram(program);
		return 0;
	}

	// Read position attribute with size of 2 and stride of 4 beginning at the start of the array. The
	// last argument indicates offset of data within the vertex buffer.
        F->glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
        F->glEnableVertexAttribArray(position);

	// Read texcoord attribute  with size of 2 and stride of 4 beginning at the first texcoord in the
	// array. The last argument indicates offset of data within the vertex buffer.
        F->glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
        F->glEnableVertexAttribArray(texcoord);

	return program;
}

bool GLVideoShader::createVertexBuffer(GLuint *vertexBuffer, GLuint *vertexArray) {
        F->glGenVertexArrays(1, vertexArray);
	if (*vertexArray == 0) {
		return false;
	}
        F->glBindVertexArray(*vertexArray);

        F->glGenBuffers(1, vertexBuffer);
	if (*vertexBuffer == 0) {
                F->glDeleteVertexArrays(1, vertexArray);
		return false;
	}
        F->glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
        F->glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	return true;
}

// Set vertex data to the currently bound vertex buffer.
void GLVideoShader::setVertexData(webrtc::VideoRotation rotation) {
	// When modelview and projection matrices are identity (default) the world is
	// contained in the square around origin with unit size 2. Drawing to these
	// coordinates is equivalent to drawing to the entire screen. The texture is
	// stretched over that square using texture coordinates (u, v) that range
	// from (0, 0) to (1, 1) inclusive. Texture coordinates are flipped vertically
	// here because the incoming frame has origin in upper left hand corner but
	// OpenGL expects origin in bottom left corner.
	std::array<std::array<GLfloat, 2>, 4> UVCoords = { {
		{{0, 1}},  // Lower left.
		{{1, 1}},  // Lower right.
		{{1, 0}},  // Upper right.
		{{0, 0}},  // Upper left.
	} };

	// Rotate the UV coordinates.
	int rotation_offset;
	switch (rotation) {
	case webrtc::kVideoRotation_0:
		rotation_offset = 0;
		break;
	case webrtc::kVideoRotation_90:
		rotation_offset = 1;
		break;
	case webrtc::kVideoRotation_180:
		rotation_offset = 2;
		break;
	case webrtc::kVideoRotation_270:
		rotation_offset = 3;
		break;
	}
	std::rotate(UVCoords.begin(), UVCoords.begin() + rotation_offset,
		UVCoords.end());

	const GLfloat gVertices[] = {
		// X, Y, U, V.
		-1, -1, UVCoords[0][0], UVCoords[0][1],
		 1, -1, UVCoords[1][0], UVCoords[1][1],
		 1,  1, UVCoords[2][0], UVCoords[2][1],
		-1,  1, UVCoords[3][0], UVCoords[3][1],
	};

        F->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(gVertices), gVertices);
}

bool GLVideoShader::createAndSetupI420Program() {
	assert(!_i420Program);
	_i420Program = createProgramFromFragmentSource(kI420FragmentShaderSource);
	if (!_i420Program) {
		return false;
	}
        GLint ySampler = F->glGetUniformLocation(_i420Program, "s_textureY");
        GLint uSampler =F-> glGetUniformLocation(_i420Program, "s_textureU");
        GLint vSampler =F-> glGetUniformLocation(_i420Program, "s_textureV");

	if (ySampler < 0 || uSampler < 0 || vSampler < 0) {
                qDebug("Failed to get uniform variable locations in I420 shader");
                F->glDeleteProgram(_i420Program);
		_i420Program = 0;
		return false;
	}

        F->glUseProgram(_i420Program);
        F->glUniform1i(ySampler, kYTextureUnit);
        F->glUniform1i(uSampler, kUTextureUnit);
        F->glUniform1i(vSampler, kVTextureUnit);

	return true;
}

bool GLVideoShader::createAndSetupNV12Program() {
	assert(!_nv12Program);
	_nv12Program = createProgramFromFragmentSource(kNV12FragmentShaderSource);
	if (!_nv12Program) {
		return false;
	}
        GLint ySampler =F->glGetUniformLocation(_nv12Program, "s_textureY");
        GLint uvSampler =F-> glGetUniformLocation(_nv12Program, "s_textureUV");

	if (ySampler < 0 || uvSampler < 0) {
                qDebug("Failed to get uniform variable locations in NV12 shader");
                F->glDeleteProgram(_nv12Program);
		_nv12Program = 0;
		return false;
	}

        F->glUseProgram(_nv12Program);
        F->glUniform1i(ySampler, kYTextureUnit);
        F->glUniform1i(uvSampler, kUvTextureUnit);

	return true;
}

bool GLVideoShader::prepareVertexBuffer(webrtc::VideoRotation rotation) {
	if (!_vertexBuffer && !createVertexBuffer(&_vertexBuffer, &_vertexArray)) {
                qDebug("Failed to setup vertex buffer");
		return false;
	}

        F->glBindVertexArray(_vertexArray);

        F->glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	if (!_currentRotation || rotation != *_currentRotation) {
		_currentRotation = absl::optional<webrtc::VideoRotation>(rotation);
		setVertexData(*_currentRotation);
	}
	return true;
}

void GLVideoShader::applyShadingForFrame(int width, 
	int height,
	webrtc::VideoRotation rotation,
	GLuint yPlane,
	GLuint uPlane,
	GLuint vPlane) {
	if (!prepareVertexBuffer(rotation)) {
		return;
	}

	if (!_i420Program && !createAndSetupI420Program()) {
                qDebug("Failed to setup I420 program");
		return;
	}

        F->glUseProgram(_i420Program);

        F->glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kYTextureUnit));
        F->glBindTexture(GL_TEXTURE_2D, yPlane);

        F->glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kUTextureUnit));
        F->glBindTexture(GL_TEXTURE_2D, uPlane);

        F->glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kVTextureUnit));
	glBindTexture(GL_TEXTURE_2D, vPlane);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLVideoShader::applyShadingForFrame(int width,
	int height,
	webrtc::VideoRotation rotation,
	GLuint yPlane,
	GLuint uvPlane) {
	if (!prepareVertexBuffer(rotation)) {
		return;
	}

	if (!_nv12Program && !createAndSetupNV12Program()) {
                qDebug("Failed to setup NV12 shader");
		return;
	}

        F->glUseProgram(_nv12Program);

        F->glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kYTextureUnit));
	glBindTexture(GL_TEXTURE_2D, yPlane);

        F->glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + kUvTextureUnit));
	glBindTexture(GL_TEXTURE_2D, uvPlane);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


