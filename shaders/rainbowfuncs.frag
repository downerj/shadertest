#version 330

#pragma include "base.part.frag"
#pragma include "colors.part.frag"

#define CYCLE_SPEED 1./5.
#define FUNC_CIRCLES

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = resolution*.5;
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

  vec3 hsv = vec3(value, 1., 1.);
  fragColor = hsvCycled2rgba(hsv, 5., CYCLE_SPEED);
}
