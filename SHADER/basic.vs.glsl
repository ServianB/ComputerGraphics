#version 120 // Utilisation de GLSL 1.20 pour OpenGL 2.1

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

varying vec3 vNormal;
varying vec2 vTexCoord;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    vNormal = aNormal;
    vTexCoord = aTexCoord;
}
