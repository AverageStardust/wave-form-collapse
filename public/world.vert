attribute vec3 position;

varying vec2 screenCoord;

void main() {
    screenCoord = position.xy * 0.5;
    gl_Position = vec4(position, 1.0);
}