import { IntList } from "./list";

declare const Module: EmscriptenModule;
declare const runtimeInitialized: boolean;

export var CHUNK_SIZE: number;
export var free: (ptr: number) => void;
export var getChunkTiles: (x: number, y: number, markDisplayed: boolean) => null | Int32Array;
export var getUndisplayedChunks: (x: number, y: number, width: number, height: number) => IntList;

export async function init() {
    await moduleReady();

    CHUNK_SIZE = Object.freeze(ccall("get_chunk_size", "number", [], []));

    free = Module._free;

    const _getChunkTiles = cwrap("get_chunk_tiles", "number", ["number", "number", "number"]);
    getChunkTiles = (x: number, y: number, markDisplayed: boolean) => {
        const tilesPtr = _getChunkTiles(x, y, Number(markDisplayed));
        if (tilesPtr == 0) return null;
        return new Int32Array(Module.HEAP32.buffer, tilesPtr, CHUNK_SIZE * CHUNK_SIZE);
    }

    const _getUndisplayedChunks = cwrap("get_undisplayed_chunks", "number", ["number", "number", "number", "number"]);
    getUndisplayedChunks = (x: number, y: number, width: number, height: number) => {
        const listPtr = _getUndisplayedChunks(x, y, width, height);
        return new IntList(listPtr);
    }
}

export function moduleReady() {
    return new Promise<void>((resolve) => {
        if ("Module" in window) {
            onModuleLoaded();
        } else {
            const script = document.querySelector<HTMLScriptElement>("#cmodule")!;
            script.addEventListener("load", onModuleLoaded);
        }

        function onModuleLoaded() {
            if (runtimeInitialized) {
                resolve();
            } else {
                Module.addOnInit(resolve);
            }
        }
    })
}