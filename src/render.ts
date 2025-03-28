import * as twgl from "twgl.js";
import { Camera } from "./camera";
import { free } from "./cwrapper";
import { World } from "./world";

const WORLD_TEX_BITS = 8;
const WORLD_TEX_SIZE = 1 << WORLD_TEX_BITS;
const WORLD_TEX_MASK = WORLD_TEX_SIZE - 1;
const TILESET_TILE_SIZE = 16;
const TILESET_WIDTH = 49;

export class Renderer {
    canvas: HTMLCanvasElement
    gl: WebGL2RenderingContext
    programInfo!: twgl.ProgramInfo
    bufferInfo: twgl.BufferInfo
    worldTexture: WebGLTexture
    tilesetTexture: WebGLTexture

    camera: Camera;
    world: World | null = null;

    frameReleventPointers: number[] = [];

    constructor(canvas: HTMLCanvasElement) {
        this.canvas = canvas;

        const gl = canvas.getContext("webgl2");
        if (!gl) throw Error("WEBGL2 not supported");
        this.gl = gl;

        this.bufferInfo = twgl.primitives.createXYQuadBufferInfo(gl);

        const textures = twgl.createTextures(gl, {
            "world": {
                width: WORLD_TEX_SIZE,
                height: WORLD_TEX_SIZE,
                minMag: gl.NEAREST,
                internalFormat: gl.R32I,
                format: gl.RED_INTEGER,
                type: gl.INT,
                src: new Int32Array(WORLD_TEX_SIZE * WORLD_TEX_SIZE).fill(-1)
            },
            "tileset": {
                minMag: gl.NEAREST,
                src: "./tileset.png"
            }
        });
        this.worldTexture = textures["world"];
        this.tilesetTexture = textures["tileset"];

        this.camera = new Camera();
    }

    async load() {
        const vertexShader = await (await fetch("./world.vert")).text();
        const fragmentShader = await (await fetch("./world.frag")).text();

        this.programInfo = twgl.createProgramInfo(this.gl, [vertexShader, fragmentShader]);
    }

    setWorld(world: World) {
        this.world = world;
    }

    start() {
        requestAnimationFrame(this.frame.bind(this));
    }

    frame(_time: number) {
        const gl = this.gl;

        twgl.resizeCanvasToDisplaySize(this.canvas);
        gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

        this.updateWorldTexture();

        const zoom = this.camera.size / Math.min(this.canvas.width, this.canvas.height);

        const uniforms = {
            resolution: [this.canvas.width, this.canvas.height],

            cameraPosition: this.camera.position.array,
            cameraZoom: zoom,

            tilesetTex: this.tilesetTexture,
            tilesetTileSize: TILESET_TILE_SIZE,
            tilesetWidth: TILESET_WIDTH,

            worldTex: this.worldTexture,
            worldTexSize: WORLD_TEX_SIZE
        }

        gl.useProgram(this.programInfo.program);
        twgl.setBuffersAndAttributes(gl, this.programInfo, this.bufferInfo);
        twgl.setUniforms(this.programInfo, uniforms);
        twgl.drawBufferInfo(gl, this.bufferInfo);

        requestAnimationFrame(this.frame.bind(this));
        gl.flush(); // just in case

        // release memory used for frame rendering
        for (const ptr of this.frameReleventPointers) free(ptr);
    }

    updateWorldTexture() {
        const gl = this.gl, world = this.world;

        if (world == null) throw Error("Can't update world texture, no world assigned");

        gl.bindTexture(gl.TEXTURE_2D, this.worldTexture);

        const zoom = this.camera.size / Math.min(this.canvas.width, this.canvas.height);
        const tilesWidth = this.canvas.width * zoom;
        const tilesHeight = this.canvas.height * zoom;
        const chuckLowX = Math.floor((this.camera.position.x - tilesWidth) / world.chunkSize);
        const chuckLowY = Math.floor((this.camera.position.y - tilesHeight) / world.chunkSize);
        const chuckHighX = Math.ceil((this.camera.position.x + tilesWidth) / world.chunkSize);
        const chuckHighY = Math.ceil((this.camera.position.y + tilesHeight) / world.chunkSize);
        const chunkWidth = chuckHighX - chuckLowX + 1;
        const chunkHeight = chuckHighY - chuckLowY + 1;

        const chunks = world.getUndisplayedChunks(chuckLowX, chuckLowY, chunkWidth, chunkHeight);

        for (const chunk of chunks) {
            const x = chunk.x * world.chunkSize;
            const y = chunk.y * world.chunkSize;
            const { data, ptr } = chunk.getRenderData();

            gl.texSubImage2D(
                gl.TEXTURE_2D, 0,
                x & WORLD_TEX_MASK, y & WORLD_TEX_MASK,
                world.chunkSize, world.chunkSize,
                gl.RED_INTEGER, gl.INT, data);

            chunk.isDisplayed = true;

            this.frameReleventPointers.push(ptr);
        }
    }
}