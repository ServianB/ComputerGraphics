#version 330 core

in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 clipPosition; // Variable d'entrée pour la position clipée

out vec4 fragColor;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - vec3(clipPosition));

    float diffuseFactor = max(dot(normal, lightDir), 0.0);

    vec3 diffuse = lightColor * objectColor * diffuseFactor;

    fragColor = vec4(diffuse, 1.0);
}
