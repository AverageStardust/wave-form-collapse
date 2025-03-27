declare const Module: EmscriptenModule;
declare const runtimeInitialized: boolean;

export var free: (ptr: number) => void;

export async function init() {
    await moduleReady();
    free = Module._free;
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