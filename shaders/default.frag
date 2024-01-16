#version 320 es

#pragma include "base.part.frag"

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  // Solid red.
  fragColor = vec4(1., 0., 0., 1.);
}
