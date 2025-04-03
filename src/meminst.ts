export let mallocInst: (size: number) => number;
export let callocInst: (nmemb: number, size: number) => number;
export let reallocInst: (ptr: number, size: number) => number;
export let freeInst: (ptr: number) => void;
export let getMemoryUsage: () => number;

export function init() {
    mallocInst = cwrap("malloc_inst", "number", ["number"]);
    callocInst = cwrap("calloc_inst", "number", ["number", "number"]);
    reallocInst = cwrap("realloc_inst", "number", ["number", "number"]);
    freeInst = cwrap("realloc_inst", null, ["number"]);
    getMemoryUsage = cwrap("get_memory_usage", "number", []);
}