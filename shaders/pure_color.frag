  #version 330 core

uniform vec3 aColor;
out vec4 FragColor;
void main() {
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); // ³ÈÉ«
    FragColor = vec4(aColor, 1.0f); // ³ÈÉ«
    // FragColor = vec4(0.2f, 0.3f, 1.0f, 1.0f); // À¶É«

}