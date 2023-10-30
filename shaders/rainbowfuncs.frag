#version 330

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
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

#define CYCLE_SPEED 1.0/5.0
#define FUNC_CIRCLES

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = resolution*0.5;
  float scale = min(resolution.x, resolution.y);
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

  vec3 hsv = vec3(value, 1.0, 1.0);
  fragColor = hsvCycled2rgba(hsv, 5.0, CYCLE_SPEED);
}

#define fragCoord gl_FragCoord
#if __VERSION__ <= 130
#define fragColor gl_FragColor
#else
out vec4 fragColor;
#endif

void main(void) {
  setColor(gl_FragColor, fragCoord);
}
