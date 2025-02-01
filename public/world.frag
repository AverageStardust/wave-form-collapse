#version 300 es

precision mediump float;

in vec2 screenCoord;
out vec4 fragColor;

uniform vec2 cameraPosition;
uniform float cameraZoom;
uniform vec2 resolution;

void main() {
    vec2 position = screenCoord * resolution * cameraZoom + cameraPosition;
    fragColor = vec4(floor(position) * 0.25, 0.0, 1.0);
}