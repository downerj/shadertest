#version 330

#if __VERSION__ > 130
#define attribute in
#define varying out
#endif

#define fragCoord gl_FragCoord
#if __VERSION__ <= 130
#define fragColor gl_FragColor
#else
out vec4 fragColor;
#endif

void main() {
  fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

