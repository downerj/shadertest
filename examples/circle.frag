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

/**
 * Main code.
 */

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = vec2(resolution)*.5;
  vec2 uv = (fragCoord.xy - c) / min(float(resolution.x), float(resolution.y));
  float d = distance(vec2(0.), uv);
  float width = .5;
  float hue = d*2. + time/10.;
  float sat = 1.;
  float val = step(-2.*width + 1., sin(d*20. + time));
  vec3 hsv = vec3(hue, sat, val);
  fragColor = hsv2rgba(hsv);
}
