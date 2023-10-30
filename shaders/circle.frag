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
  vec3 k = vec3(1., 2./3., 1./3.);
  vec3 p = clamp(abs(6.*fract(h - k) - 3.) - 1., 0., 1.);
  return vec4(v * mix(k.xxx, p, s), 1.);
}

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = resolution*0.5;
  vec2 uv = (fragCoord.xy - c) / min(resolution.x, resolution.y);
  float d = distance(vec2(0.), uv);
  float width = .5;
  float hue = d*2. + time/10.;
  float sat = 1.;
  float val = step(-2.*width + 1., sin(d*20. + time));
  vec3 hsv = vec3(hue, sat, val);
  fragColor = hsv2rgba(hsv);
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
