#version 410 core
layout(location = 0) in vec2 pos;  // [-1,1]
out vec2 TexCoords;

void main() {
    TexCoords = pos * 0.5 + 0.5;  // [-1,1] -> [0,1]
    // TexCoords = pos + vec2(1.0, 1.0);  // [-1,1] -> [0,1]
    gl_Position = vec4(pos, 0.0, 1.0);
}
