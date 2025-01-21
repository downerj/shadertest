R"(#version 110
#define fragCoordIn gl_FragCoord
#if __VERSION__ <= 130
#define fragColorOut gl_FragColor
#else
out vec4 fragColorOut;
#endif

uniform float time;
uniform ivec2 resolution;

vec4 hsv2rgba(in vec3 hsv) {
  float h = hsv.x;
  float s = hsv.y;
  float v = hsv.z;
  vec3 k = vec3(1., 2./3., 1./3.);
  vec3 p = clamp(abs(6.*fract(h - k) - 3.) - 1., 0., 1.);
  return vec4(v * mix(k.xxx, p, s), 1.);
}

void main(void) {
  vec2 center = vec2(resolution)*.5;
  float scale = min(float(resolution.x), float(resolution.y));
  vec2 newCenter = (fragCoordIn.xy - center)/scale;
  vec3 hsv = vec3(newCenter.y - newCenter.x + time/10., 1., 1.);
  fragColorOut = hsv2rgba(hsv);
}
)"
