#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture3;

void main() {
    // Define uma textura diferente para as rodas
    FragColor = texture(texture3, TexCoord);
}
