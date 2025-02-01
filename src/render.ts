import * as twgl from "twgl.js";
import { Camera } from "./camera";

export class Renderer {
    canvas: HTMLCanvasElement
    gl: WebGLRenderingContext
    programInfo!: twgl.ProgramInfo
    bufferInfo: twgl.BufferInfo
    camera: Camera

    constructor(canvas: HTMLCanvasElement) {
        this.canvas = canvas;

        const gl = canvas.getContext("webgl");
        if (gl === null) throw Error("WEBGL not supported");
        this.gl = gl;

        const arrays = {
            position: [-1, -1, 0, 1, -1, 0, -1, 1, 0, -1, 1, 0, 1, -1, 0, 1, 1, 0] //quad
        };
        this.bufferInfo = twgl.createBufferInfoFromArrays(gl, arrays);

        this.camera = new Camera();
    }

    async load() {
        const vertexShader = await (await fetch("./world.vert")).text();
        const fragmentShader = await (await fetch("./world.frag")).text();

        this.programInfo = twgl.createProgramInfo(this.gl, [vertexShader, fragmentShader]);
    }

    start() {
        requestAnimationFrame(this.frame.bind(this));
    }

    frame(_time: number) {
        const gl = this.gl;

        twgl.resizeCanvasToDisplaySize(this.canvas);
        gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

        const uniforms = {
            cameraPosition: this.camera.position.array,
            cameraZoom: this.camera.zoom,
            resolution: [this.canvas.width, this.canvas.height],
        }

        gl.useProgram(this.programInfo.program);
        twgl.setBuffersAndAttributes(gl, this.programInfo, this.bufferInfo);
        twgl.setUniforms(this.programInfo, uniforms);
        twgl.drawBufferInfo(gl, this.bufferInfo);

        requestAnimationFrame(this.frame.bind(this));
    }
}