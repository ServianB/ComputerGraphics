#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec3 fragNormal;
out vec2 fragTexCoord;
out vec4 clipPosition; // Nouvelle variable pour la position clipée

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;

    // Transformation des coordonnées du vertex
    vec4 worldPosition = model * vec4(inPosition, 1.0);
    clipPosition = projection * view * worldPosition;

    gl_Position = clipPosition;
}
