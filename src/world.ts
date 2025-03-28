import { heap32 } from "./cwrapper";
import { DistributionArea } from "./distribution";
import { List } from "./list";
import { DistributionTileset, Tileset } from "./tileset";

let world_create: (chunk_size: number, tileset_ptr: number) => number;
let world_get_undisplayed_chunks: (ptr: number, x: number, y: number, width: number, height: number) => number;
let world_get_chunk_render_data: (worldPtr: number, chunkPtr: number) => number;
let world_create_chunk: (ptr: number, x: number, y: number) => number;
let world_get_chunk: (ptr: number, x: number, y: number) => number;
let world_set: (ptr: number, x: number, y: number, tile: number) => number;
let world_get: (ptr: number, x: number, y: number) => number;
let world_free: (ptr: number) => void;

const worldRegistry = new FinalizationRegistry((ptr: number) => {
    world_free(ptr);
});

export function init() {
    world_create = cwrap("world_create", "number", ["number", "number"]);
    world_get_undisplayed_chunks = cwrap("world_get_undisplayed_chunks", "number", ["number", "number", "number", "number", "number"]);
    world_get_chunk_render_data = cwrap("world_get_chunk_render_data", "number", ["number", "number"]);
    world_create_chunk = cwrap("world_create_chunk", "number", ["number", "number", "number"]);
    world_get_chunk = cwrap("world_get_chunk", "number", ["number", "number", "number"]);
    world_set = cwrap("world_set", "number", ["number", "number", "number"]);
    world_get = cwrap("world_get", "number", ["number", "number", "number"]);
    world_free = cwrap("world_free", null, ["number"]);
}

export class World {
    readonly ptr: number;
    readonly chunkSize: number
    readonly tileset: Tileset;

    static create(chunkSize: number, tileset: Tileset, _?: number): World {
        const world = new World(world_create(chunkSize, tileset.ptr), tileset);
        worldRegistry.register(world, world.ptr);
        return world;
    }

    constructor(ptr: number, tileset: Tileset) {
        this.ptr = ptr;
        this.tileset = tileset;
        this.chunkSize = getValue(this.ptr + 0, "i32");;
    }

    getUndisplayedChunks(x: number, y: number, width: number, height: number): Chunk[] {
        const chunkPtrs = new List(world_get_undisplayed_chunks(this.ptr, x, y, width, height), 8);
        const chunks = [];

        for (let i = 0; i < chunkPtrs.length; i++) {
            chunks.push(new Chunk(chunkPtrs.at(i), this));
        }

        chunkPtrs.free();
        return chunks;
    }

    createChunk(x: number, y: number): Chunk {
        return new Chunk(world_create_chunk(this.ptr, x, y), this);
    }

    getChunk(x: number, y: number): Chunk {
        return new Chunk(world_get_chunk(this.ptr, x, y), this)
    }

    set(x: number, y: number, tileId: number): boolean {
        const success = world_set(this.ptr, x, y, tileId) !== 0;
        return success;
    }

    get(x: number, y: number): number {
        return world_get(this.ptr, x, y);
    }
}

export class DistributionWorld extends World {
    declare readonly tileset: DistributionTileset;
    distributionSize: number;

    static create(chunkSize: number, tileset: DistributionTileset, distributionSize: number): DistributionWorld {
        const world = new DistributionWorld(world_create(chunkSize, tileset.ptr), tileset, distributionSize);
        worldRegistry.register(world, world.ptr);
        return world;
    }

    constructor(ptr: number, tileset: DistributionTileset, distributionSize: number) {
        super(ptr, tileset);
        this.distributionSize = distributionSize;
    }

    getArea(x: number, y: number, width: number): DistributionArea {
        const distributions = [];

        const transformedX = x / this.distributionSize;
        const transformedY = y / this.distributionSize;
        const transformedWidth = width / this.distributionSize;

        if (Math.trunc(transformedX) != transformedX || Math.trunc(transformedY) != transformedY || Math.trunc(transformedWidth) != transformedWidth) {
            throw Error("Could not get misaligned distribution world area");
        }

        for (let v = transformedY; v < transformedY + transformedWidth; v++) {
            for (let u = transformedX; u < transformedX + transformedWidth; u++) {
                const tileId = this.get(u, v);
                distributions.push(this.tileset.getDistribution(tileId));
            }
        }

        return DistributionArea.create(distributions, this.distributionSize, transformedWidth);
    }
}

export class Chunk {
    readonly ptr: number;
    readonly world: World;
    readonly x: number;
    readonly y: number;

    constructor(ptr: number, world: World) {
        this.ptr = ptr;
        this.world = world;
        this.x = getValue(this.ptr + 0, "i32");
        this.y = getValue(this.ptr + 4, "i32");
    }

    get isDisplayed() {
        return getValue(this.ptr + 8, "i32") == 1;
    }

    set isDisplayed(isDisplayed: boolean) {
        setValue(this.ptr + 8, isDisplayed ? 1 : 0, "i32");
    }

    getRenderData(): { data: Int32Array, ptr: number } {
        const ptr = world_get_chunk_render_data(this.world.ptr, this.ptr);
        const tileArea = this.world.chunkSize ** 2;
        const data = heap32.subarray((ptr >> 2), (ptr >> 2) + tileArea);
        return { data, ptr };
    }
}