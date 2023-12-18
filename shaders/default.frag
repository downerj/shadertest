#version 330

/**
 * Setup.
 */

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

/**
 * Main code.
 */

void setColor(out vec4 fragColor, in vec4 fragCoord) {
  // Solid red.
  fragColor = vec4(1., 0., 0., 1.);
}
