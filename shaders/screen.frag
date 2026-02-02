#version 410 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D ScreenTex;

float linear_to_srgb(float c) {
    // Reinhard tone mapping
    c /= 1 + c;

    // Gamma correction
    if (c <= 0.0031308)
        c = 12.92 * c;
    else
        c = 1.055 * pow(c, 1.0 / 2.4) - 0.055;

    // Clamping
    c = clamp(c, 0.0, 1.0);
    return c;
}

vec3 linear_to_srgb(vec3 linear) {
    vec3 srgb;
    srgb.x = linear_to_srgb(linear.x);
    srgb.y = linear_to_srgb(linear.y);
    srgb.z = linear_to_srgb(linear.z);
    return srgb;
}

void main() {
    vec3 color = texture(ScreenTex, TexCoords).rgb;
    FragColor = vec4(linear_to_srgb(color), 1.0);
}
