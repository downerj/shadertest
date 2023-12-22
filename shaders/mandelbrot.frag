#version 330

#pragma include "base.part.frag"
#pragma include "colors.part.frag"

int mandelbrot(in vec2 p) {
  vec2 t = vec2(0., 0.);
  const int maxIterations = 1000;
  for (int i = 0; i < maxIterations - 1; i++) {
    if (t.x*t.x + t.y*t.y > 4.) {
      return i;
    }
    t = vec2(t.x*t.x - t.y*t.y, 2.*t.x*t.y) + p;
  }
  return -1;
}

struct Params {
  vec2 offset;
  float zoom;
};

#define PARAMS_1
#ifdef PARAMS_1
Params params = Params(vec2(-1., .3), 1.);
#else
Params params = Params(vec2(-1., 0.), -.5);
#endif

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = resolution*.5;
  float scale = resolution.y;
  vec2 uv = fragCoord.xy;
  vec2 p = (uv - c)/(scale*pow(10., params.zoom)) + params.offset;
  int iterations = mandelbrot(p);
  if (iterations < 0) {
    fragColor = vec4(0., 0., 0., 1.);
    return;
  }
  float value = float(iterations)/36.;
  vec3 hsv = vec3(value, 1., 1.);
  fragColor = hsvCycled2rgba(hsv, 1., .25);
}
