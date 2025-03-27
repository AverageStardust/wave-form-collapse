import { IntList } from "./list";

let world_create: (chunk_size: number) => number;
let world_get_undisplayed_chunks: (ptr: number, x: number, y: number, width: number, height: number) => number;
let world_create_chunk: (ptr: number, x: number, y: number) => number;
let world_get_chunk: (ptr: number, x: number, y: number) => number;
let world_set: (ptr: number, x: number, y: number, tile: number) => number;
let world_get: (ptr: number, x: number, y: number) => number;
let world_free: (ptr: number) => void;

const listRegistry = new FinalizationRegistry((ptr: number) => {
    world_free(ptr);
});

export function init() {
    world_create = cwrap("world_create", "number", ["number"]);
    world_get_undisplayed_chunks = cwrap("world_get_undisplayed_chunks", "number", ["number", "number", "number", "number", "number"]);
    world_create_chunk = cwrap("world_create_chunk", "number", ["number", "number", "number"]);
    world_get_chunk = cwrap("world_get_chunk", "number", ["number", "number", "number"]);
    world_set = cwrap("world_set", "number", ["number", "number", "number"]);
    world_get = cwrap("world_get", "number", ["number", "number", "number"]);
    world_free = cwrap("world_free", null, ["number"]);
}

export class World {
    ptr: number;

    static create(chunkSize: number) {
        const list = new World(world_create(chunkSize));
        listRegistry.register(list, list.ptr);
    }

    constructor(ptr: number) {
        this.ptr = ptr;
    }

    getUndisplayedChunks(x: number, y: number, width: number, height: number): Chunk[] {
        const chunkPtrs = new IntList(world_get_undisplayed_chunks(this.ptr, x, y, width, height));
        const chunks = [];

        for (let i = 0; i < chunkPtrs.length; i++) {
            chunks.push(new Chunk(chunkPtrs.at(i)));
        }

        chunkPtrs.free();
        return chunks;
    }

    createChunk(x: number, y: number): Chunk {
        return new Chunk(world_create_chunk(this.ptr, x, y));
    }

    getChunk(x: number, y: number): Chunk {
        return new Chunk(world_get_chunk(this.ptr, x, y))
    }

    set(x: number, y: number, tile: number): boolean {
        const success = world_set(this.ptr, x, y, tile) !== 0;
        return success;
    }

    get(x: number, y: number): number {
        return world_get(this.ptr, x, y);
    }
}

export class Chunk {
    ptr: number;

    constructor(ptr: number) {
        this.ptr = ptr;
    }

    get x() {
        return getValue(this.ptr + 0, "i32");
    }

    get y() {
        return getValue(this.ptr + 4, "i32");
    }

    get is_displayed() {
        return getValue(this.ptr + 8, "i32");
    }
}