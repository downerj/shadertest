#ifndef CORE_HH
#define CORE_HH

#include "gl-includes.hh"

namespace graphics {
#ifdef DEBUG
  void GLAPIENTRY messageCallback(
    GLenum /*source*/,
    GLenum /*type*/,
    GLuint /*id*/,
    GLenum /*severity*/,
    GLsizei /*length*/,
    const GLchar* message,
    const void* /*userParam*/
  );
#endif // DEBUG

  std::tuple<const GLubyte*, const GLubyte*, const GLubyte*, const GLubyte*> getInfo();
  void printInfo();
  std::tuple<int, int> parseGLVersion(const std::string& arg);
  bool doesGLSLVersionUseInOut(const std::string& versionLine);
  GLuint createBuffer(GLenum target, size_t size, void* data, GLenum usage);
  GLuint createShader(GLenum type, const std::string& source);
  GLuint createProgram(const std::string& vertexSource, const std::string& fragmentSource);
  std::string initializeVertexSource(const std::string& fragmentSource);
}

#endif // CORE_HH
