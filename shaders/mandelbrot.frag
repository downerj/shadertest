#version 330

#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

#if __VERSION__ > 130
#define attribute in
#define varying out
#endif

uniform vec2 resolution;
uniform float time;

vec4 hsv2rgba(in vec3 hsv) {
  float h = hsv.x;
  float s = hsv.y;
  float v = hsv.z;
  vec3 k = vec3(1.0, 2.0/3.0, 1.0/3.0);
  vec3 p = clamp(abs(6.0*fract(h - k) - 3.0) - 1.0, 0.0, 1.0);
  return vec4(v * mix(k.xxx, p, s), 1.0);
}

vec4 hsvCycled2rgba(in vec3 hsv, in float spread, in float speed) {
  hsv.x = fract(hsv.x*spread + time*speed);
  return hsv2rgba(hsv);
}

int mandelbrot(in vec2 p) {
  vec2 t = vec2(0.0, 0.0);
  const int maxIterations = 1000;
  for (int i = 0; i < maxIterations - 1; i++) {
    if (t.x*t.x + t.y*t.y > 4.0) {
      return i;
    }
    t = vec2(t.x*t.x - t.y*t.y, 2.0*t.x*t.y) + p;
  }
  return -1;
}

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = resolution*0.5;
  float scale = resolution.y;
  vec2 uv = fragCoord.xy;

  const vec2 offset = vec2(-1.0, 0.3);
  const float zoom = 1.0;
  
  // Standard view.
  // vec2 offset = vec2(-1.0, 0.0);
  // const float zoom = -0.5;

  vec2 p = (uv - c)/(scale*pow(10.0, zoom)) + offset;

  int iterations = mandelbrot(p);
  if (iterations < 0) {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  float value = float(iterations)/36.0;
  vec3 hsv = vec3(value, 1.0, 1.0);
  fragColor = hsvCycled2rgba(hsv, 1.0, 0.25);
}

#define fragCoord gl_FragCoord
#if __VERSION__ <= 130
#define fragColor gl_FragColor
#else
out vec4 fragColor;
#endif

void main() {
  setColor(fragColor, fragCoord);
}
