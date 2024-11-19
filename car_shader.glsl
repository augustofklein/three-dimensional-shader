#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture2;

void main() {
    // Define uma cor distinta para o carro
    FragColor = texture(texture2, TexCoord);
    //FragColor = vec4(0.0, 1.0, 1.0, 1.0); // Cor ciano
}
