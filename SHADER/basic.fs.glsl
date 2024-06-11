#version 120 // Utilisation de GLSL 1.20 pour OpenGL 2.1

varying vec3 vNormal;
varying vec2 vTexCoord;

uniform sampler2D uTexture;
uniform vec3 uLightDirection;

void main() {
    float brightness = max(dot(normalize(vNormal), normalize(uLightDirection)), 0.0);
    vec4 texColor = texture2D(uTexture, vTexCoord);
    gl_FragColor = texColor * brightness;
}
