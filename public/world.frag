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
    int renderData = texelFetch(worldTex, ivec2(floor(position)) % worldTexSize, 0).r;
    if (renderData < 0) return vec4(0.0);

    int id = renderData & 0xFFFF;
    int transformation = renderData >> 16 & 0x7;
    
    vec2 tileSubPosition = vec2(fract(position.x), 1.0 - fract(position.y));

    if ((transformation & 0x4) != 0) {
        float temp = tileSubPosition.x;
        tileSubPosition.x = tileSubPosition.y;
        tileSubPosition.y = temp;
    }

    if ((transformation & 0x2) != 0) {
        tileSubPosition.y = 1.0 - tileSubPosition.y;
    }

    if ((transformation & 0x1) != 0) {
        tileSubPosition.x = 1.0 - tileSubPosition.x;
    }

    vec2 tileCoord = vec2(id % tilesetWidth, id / tilesetWidth) + tileSubPosition;
    return texelFetch(tilesetTex, ivec2(tileCoord * float(tilesetTileSize)), 0);
}

void main() {
    vec2 position = screenCoord * resolution * cameraZoom + cameraPosition;
    fragColor = sampleWorld(position);
}