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
