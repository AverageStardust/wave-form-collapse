let tileset_create: (edge_field_size: number, tile_field_size: number) => number;
let tileset_add_tile: (tileset: number, tile: number, render_data: number, right_edge: number, top_edge: number, left_edge: number, bottom_edge: number) => void;
let tileset_free: (ptr: number) => null;

const tilesetRegistry = new FinalizationRegistry((ptr: number) => {
    tileset_free(ptr);
});

export function init() {
    tileset_create = cwrap("tileset_create", "number", ["number", "number"]);
    tileset_add_tile = cwrap("tileset_add_tile", null, ["number", "number", "number", "number", "number", "number", "number"]);
    tileset_free = cwrap("tileset_free", null, ["number"]);
}

export class Tileset {
    readonly ptr: number;
    readonly edgeLimit: number;
    readonly tileLimit: number;

    tileCount = 0;
    tileDictonary: Map<string, number> = new Map();

    static create(edgeLimit: number = 128, tileLimit: number = 128): Tileset {
        const tileset = new Tileset(tileset_create((edgeLimit + 7) >> 3, (tileLimit + 7) >> 3));
        tilesetRegistry.register(tileset, tileset.ptr);
        return tileset;
    }

    constructor(ptr: number) {
        this.ptr = ptr;
        this.edgeLimit = getValue(this.ptr + 0, "i32") * 8;
        this.tileLimit = getValue(this.ptr + 4, "i32") * 8;
    }

    addTile(name: string, textureId: number, rightEdge: number, topEdge: number, leftEdge: number, bottomEdge: number) {
        if (this.tileCount >= this.tileLimit)
            throw Error("Tileset has hit tile limit");

        const renderData = textureId;
        tileset_add_tile(this.ptr, this.tileCount, renderData, rightEdge, topEdge, leftEdge, bottomEdge);

        this.tileDictonary.set(name, this.tileCount);
        this.tileCount++;
    }

    getTile(name: string) {
        const tileId = this.tileDictonary.get(name);
        if (tileId === undefined) throw Error(`Can't find tile "${name}"`);
        return tileId;
    }
}