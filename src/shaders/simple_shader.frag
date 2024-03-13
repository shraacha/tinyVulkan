#version 450

layout (location = 0) out vec4 outColor;

// SUS gradient
// vec2 viewPortSize = vec2(800, 600);
// vec2 pixelNormalized = gl_FragCoord.xy / viewPortSize;

void main() {
    //outColor = vec4(pixelNormalized.x, pixelNormalized.y, 0.5, 1.0);

    outColor = vec4(0.1, 1.0, 0.05, 1.0);
}
