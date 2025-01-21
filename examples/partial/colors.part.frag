vec4 hsv2rgba(in vec3 hsv) {
  float h = hsv.x;
  float s = hsv.y;
  float v = hsv.z;
  vec3 k = vec3(1., 2./3., 1./3.);
  vec3 p = clamp(abs(6.*fract(h - k) - 3.) - 1., 0., 1.);
  return vec4(v * mix(k.xxx, p, s), 1.);
}

vec4 hsvCycled2rgba(in vec3 hsv, in float spread, in float speed) {
  hsv.x = fract(hsv.x*spread + time*speed);
  return hsv2rgba(hsv);
}
