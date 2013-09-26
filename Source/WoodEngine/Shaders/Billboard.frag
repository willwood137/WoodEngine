uniform sampler2D u_diffuse;

varying vec2 v_texCoords;

/**
 * Pixel Shader
 */
void main() {
  gl_FragColor = texture2D(u_diffuse, v_texCoords);
}
