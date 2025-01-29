precision mediump float;

varying vec2 screenCoord;

uniform vec2 cameraPosition;
uniform float cameraZoom;
uniform vec2 resolution;

void main() {
    vec2 position = screenCoord * resolution * cameraZoom + cameraPosition;
    gl_FragColor = vec4(floor(position) * 0.25, 0.0, 1.0);
}