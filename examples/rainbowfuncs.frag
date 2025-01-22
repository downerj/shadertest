#version 320 es

#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
precision highp int;
#else
precision mediump float;
precision mediump int;
#endif
#endif

#if __VERSION__ > 130
#define attribute in
#define varying out
#endif

uniform ivec2 resolution;
uniform float time;

#define fragCoordIn gl_FragCoord
#if __VERSION__ <= 130
#define fragColorOut gl_FragColor
#else
out vec4 fragColorOut;
#endif

void setColor(out vec4, in vec4);
void main(void) {
  setColor(fragColorOut, fragCoordIn);
}

vec4 hsv2rgba(in vec3 hsv) {
  float h = hsv.x;
  float s = hsv.y;
  float v = hsv.z;
  vec3 k = vec3(1., 2./3., 1./3.);
  vec3 p = clamp(abs(6.*fract(h - k) - 3.) - 1., 0., 1.);
  return vec4(v * mix(k.xxx, p, s), 1.);
}

vec4 hsvCycled2rgba(in vec3 hsv, in float spread, in float speed) {
  hsv.x = fract(hsv.x*spread + time*speed);
  return hsv2rgba(hsv);
}

#define CYCLE_SPEED 1./5.
#define FUNC_CIRCLES

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = vec2(resolution)*.5;
  float scale = min(float(resolution.x), float(resolution.y));
  vec2 p = (fragCoord.xy - c)/scale;

#ifdef FUNC_CIRCLES
  float value = sqrt(p.x*p.x + p.y*p.y);
#elif defined FUNC_LINES
  float value = p.y - p.x;
#elif defined FUNC_HYPERBOLAS_CHAOTIC
  float value = p.x*p.x/p.y - p.y;
#elif defined FUNC_HYPERBOLAS
  float value = sqrt(abs(p.x*p.x - p.y*p.y));
#elif defined FUNC_PARABOLAS_CHAOTIC
  float value = p.x*p.x/p.y;
#endif

  vec3 hsv = vec3(value, 1., 1.);
  fragColor = hsvCycled2rgba(hsv, 5., CYCLE_SPEED);
}
