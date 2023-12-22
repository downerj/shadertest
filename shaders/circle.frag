#version 330

#pragma include "base.part.frag"
#pragma include "colors.part.frag"

/**
 * Main code.
 */

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  vec2 c = resolution*.5;
  vec2 uv = (fragCoord.xy - c) / min(resolution.x, resolution.y);
  float d = distance(vec2(0.), uv);
  float width = .5;
  float hue = d*2. + time/10.;
  float sat = 1.;
  float val = step(-2.*width + 1., sin(d*20. + time));
  vec3 hsv = vec3(hue, sat, val);
  fragColor = hsv2rgba(hsv);
}
