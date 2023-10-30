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

// **********
// * Uniforms
// **********

uniform vec2 resolution;
uniform float time;
uniform vec2 mouse;

// **********
// * Math
// **********

#define E 2.71828182845904
#define PI 3.141592653589793
#define DEG_TO_RAD PI/180.
#define RAD_TO_DEG 180./PI

const vec2 R = vec2(1., 0.);
const vec2 I = vec2(0., 1.);

vec2 cmul(in vec2 a, in vec2 b) {
  return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

vec2 cdiv(in vec2 a, in vec2 b) {
  float d = b.x*b.x + b.y*b.y;
  return vec2((a.x*b.x + a.y*b.y)/d, (a.y*b.x - a.x*b.y)/d);
}

#define MAX_N 10
vec2 cpowz(in vec2 z, in int n) {
  if (n == 0) {
    return R;
  }
  vec2 res = R;
  // Custom replacement for int abs(int) for GLSL 1.00.
  //int limit = n < 0 ? -n : n;
  int limit = abs(n);
  for (int i = 0; i < MAX_N; i++) {
    if (i >= limit) {
      break;
    }
    if (n > 1) {
      res = cmul(res, z);
    } else {
      res = cdiv(res, z);
    }
  }
  return res;
}

float clen(in vec2 z) {
  return sqrt(z.x*z.x + z.y*z.y);
}

float carg(in vec2 z) {
  return atan(z.y, z.x);
}

vec2 cpolar(in float r, in float th) {
  return vec2(r*cos(th), r*sin(th));
}

vec2 cunit(in vec2 z) {
  float d = sqrt(z.x*z.x + z.y*z.y);
  return vec2(z.x/d, z.y/d);
}

vec2 crotate(in vec2 z, in float degrees) {
  float r = clen(z);
  float th = carg(z);
  return cpolar(r, th + degrees*DEG_TO_RAD);
}

vec2 ccos(in vec2 z) {
  return vec2(
    cos(z.x) * cosh(z.y),
    -1. * sin(z.x) * sinh(z.y)
  );
}

vec2 csin(in vec2 z) {
  return vec2(
    sin(z.x) * cosh(z.y),
    cos(z.x) * sinh(z.y)
  );
}

vec2 ctan(in vec2 z) {
  vec2 numer = vec2(tan(z.x), tanh(z.y));
  vec2 denom = vec2(1.0, -1.0 * tan(z.x) * tanh(z.y));
  return cdiv(numer, denom);
}

// **********
// * Colors
// **********

#define DO_HUE_CYCLE
#ifdef DO_HUE_CYCLE
#define HUE_SPREAD 2.
#define HUE_SPEED .1
#endif

vec4 hsv2rgba(in vec3 hsv) {
#ifdef DO_HUE_CYCLE
  float h = fract(hsv.x*HUE_SPREAD + time*HUE_SPEED);
#else
  float h = hsv.x;
#endif
  float s = hsv.y;
  float v = hsv.z;
  vec3 k = vec3(1., 2./3., 1./3.);
  vec3 p = clamp(abs(6.*fract(h - k) - 3.) - 1., 0., 1.);
  return vec4(v * mix(k.xxx, p, s), 1.);
}

#define SATURATION_RATIO .0625
#define CONTOURS
#ifdef CONTOURS

#endif

vec3 complex2hsv(vec2 point) {
  float x = point.x;
  float y = point.y;
  float r = sqrt(x*x + y*y);
  float a = atan(y, x)*RAD_TO_DEG;

  float hue = a/360.;
  float sat = 1. - pow(SATURATION_RATIO, r);
#ifndef CONTOURS
  return vec3(hue, sat, 1.);
#else
  // Repeated edges.
  float val = 1. - .25*mod(r*.2, 2.);
  
  // Checker grid.
  if (step(sin(x*5.), 0.) == step(sin(y*5.), 0.)) {
    val += .2;
  }
  
  return vec3(hue, sat, val);
#endif
}

// **********
// * Primary complex function(s)
// **********

#define OFFSET_X 0.
#define OFFSET_Y 0.
#define SCALE .10

#define FUNCTION_A

#ifdef FUNCTION_A
vec2 func(vec2 z) {
  vec2 a = cmul(z, z) - R;
  vec2 b = z - 2.*R - I;
  vec2 b2 = cmul(b, b);
  vec2 d = cmul(z, z) + 2.*R + 2.*I;
  vec2 o = cdiv(cmul(a, b2), d);
  return o;
}
#elif defined FUNCTION_B
vec2 func(vec2 z) {
  return 2.*I + R + cmul(z, z);
}
#elif defined FUNCTION_C
vec2 func(vec2 z) {
  return cpowz(z, 3) - 25.*R;
}
#endif

// **********
// * Main
// **********

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = resolution.xy*.5 + vec2(OFFSET_X, OFFSET_Y);
  float scale = SCALE*min(resolution.x, resolution.y);
  vec2 z = (fragCoord.xy - c)/scale;
  vec2 o = func(z);
  vec3 hsv = complex2hsv(o);
  fragColor = hsv2rgba(hsv);
}

out vec4 fragColor;

void main(void) {
  setColor(fragColor, gl_FragCoord);
}

