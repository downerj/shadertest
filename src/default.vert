R"(#version 110
#define positionOut gl_Position
#if __VERSION__ <= 130
attribute vec3 position;
#else
in vec3 position;
#endif

uniform float time;
uniform ivec2 resolution;

void main(void) {
  positionOut = vec4(position, 1.);
}
)"
