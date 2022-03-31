#pragma once

#include <string>

std::string vertexShaderSource(R"str(#version 100
precision highp float;

attribute vec2 vertex;

void main() {
  gl_Position = vec4(vertex, 0.0, 1.0);
}
)str");

std::string fragmentDefaultShaderSource(R"str(#version 100
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

#define PI 3.141592653589793
#define DEG_TO_RAD PI/180.0
#define RAD_TO_DEG 180.0/PI

vec4 hsv2rgba(in vec3 hsv) {
  float h = hsv.x;
  float s = hsv.y;
  float v = hsv.z;
  vec3 k = vec3(1.0, 2.0/3.0, 1.0/3.0);
  vec3 p = clamp(abs(6.0*fract(h - k) - 3.0) - 1.0, 0.0, 1.0);
  return vec4(v * mix(k.xxx, p, s), 1.0);
}

uniform vec2 resolution;
uniform float time;

void main() {
  float hue = time*0.1;
  gl_FragColor = hsv2rgba(vec3(hue, 1.0, 1.0));
}
)str");

