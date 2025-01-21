#version 320 es

#define positionOut gl_Position
#if __VERSION__ <= 130
attribute vec3 position;
#else
in vec3 position;
#endif

precision highp float;
precision highp int;

uniform ivec2 resolution;
uniform float time;

void main(void) {
  positionOut = vec4(position, 1.);
}