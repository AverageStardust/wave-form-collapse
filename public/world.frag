#version 300 es

precision highp float;
precision highp isampler2D;

in vec2 screenCoord;
out vec4 fragColor;

uniform vec2 resolution;

uniform vec2 cameraPosition;
uniform float cameraZoom;

uniform sampler2D tilesetTex;
uniform int tilesetTileSize;
uniform int tilesetWidth;

uniform isampler2D worldTex;
uniform int worldTexSize;

vec4 sampleWorld(vec2 position) {
    int id = texelFetch(worldTex, ivec2(floor(position)) % worldTexSize, 0).r;
    if (id < 0) return vec4(0.0);
    
    vec2 tileSubPosition = vec2(fract(position.x), 1.0 - fract(position.y));
    vec2 tileCoord = vec2(id % tilesetWidth, id / tilesetWidth) + tileSubPosition;
    return texelFetch(tilesetTex, ivec2(tileCoord * float(tilesetTileSize)), 0);
}

void main() {
    vec2 position = screenCoord * resolution * cameraZoom + cameraPosition;
    fragColor = sampleWorld(position);
}