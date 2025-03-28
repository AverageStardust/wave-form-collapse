import { init as initList } from "./list.ts";
import { init as initTileset } from "./tileset.ts";
import { init as initWorld } from "./world.ts";

declare const Module: EmscriptenModule;
declare const runtimeInitialized: boolean;

export let free: (ptr: number) => void;
export let heap8: Int8Array;
export let heapU8: Uint8Array;
export let heap32: Int32Array;
export let heapU32: Uint32Array;

export async function init() {
    await moduleReady();
    free = Module._free;
    heap8 = Module.HEAP8;
    heapU8 = Module.HEAPU8;
    heap32 = Module.HEAP32;
    heapU32 = Module.HEAPU32;
    initList();
    initTileset();
    initWorld();
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