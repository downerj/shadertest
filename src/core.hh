#ifndef CORE_HH
#define CORE_HH

#include <string>
#include <tuple>

#include "gl-includes.hh"

namespace graphics {
#ifdef DEBUG
  auto GLAPIENTRY messageCallback(
    GLenum /*source*/,
    GLenum /*type*/,
    GLuint /*id*/,
    GLenum /*severity*/,
    GLsizei /*length*/,
    const GLchar* message,
    const void* /*userParam*/
  ) -> void;
#endif // DEBUG

  auto getInfo() -> std::tuple<std::string, std::string, std::string, std::string>;
  auto printInfo() -> void;
  auto parseGLVersion(const std::string& arg) -> std::tuple<int, int>;
  auto doesGLSLVersionUseInOut(const std::string& versionLine) -> bool;
  auto createBuffer(GLenum target, size_t size, void* data, GLenum usage) -> GLuint;
  auto createShader(GLenum type, const std::string& source) -> GLuint;
  auto createProgram(const std::string& vertexSource, const std::string& fragmentSource) -> GLuint;
  auto initializeVertexSource(const std::string& fragmentSource) -> std::string;
}

#endif // CORE_HH
