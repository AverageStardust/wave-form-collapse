import { mallocInst } from "./meminst";
import { Tileset } from "./tileset";

let distribution_create: (tile_field_size: number) => number;
let distribution_add_tile: (distribution: number, tile: number, weight: number) => void;
let distribution_free: (distribution: number) => void;

let distribution_area_create: (distributions: number, distribution_size: number, distributions_width: number) => number;
let distribution_area_free: (area: number) => void;

const distributionRegistry = new FinalizationRegistry((ptr: number) => {
    distribution_free(ptr);
});

const distributionAreaRegistry = new FinalizationRegistry((ptr: number) => {
    distribution_area_free(ptr);
});

export function init() {
    distribution_create = cwrap("distribution_create", "number", ["number"]);
    distribution_add_tile = cwrap("distribution_add_tile", null, ["number", "number", "number"]);
    distribution_free = cwrap("distribution_free", null, ["number"]);

    distribution_area_create = cwrap("distribution_area_create", "number", ["number", "number", "number"]);
    distribution_area_free = cwrap("distribution_area_free", null, ["number"]);
}

export class Distribution {
    readonly ptr: number;
    readonly tileset: Tileset;

    static create(tileset: Tileset): Distribution {
        const distribution = new Distribution(distribution_create((tileset.tileLimit + 7) >> 3), tileset);
        distributionRegistry.register(distribution, distribution.ptr, distribution);
        return distribution;
    }

    constructor(ptr: number, tileset: Tileset) {
        this.ptr = ptr;
        this.tileset = tileset;
    }

    addTile(tileId: number, weight: number) {
        distribution_add_tile(this.ptr, tileId, weight);
    }

    getArea(): DistributionArea {
        return DistributionArea.create([this], 2 ** 31 - 1, 1);
    }

    free() {
        distributionRegistry.unregister(this);
        distribution_free(this.ptr);
    }
}

export class DistributionArea {
    readonly ptr: number;
    readonly distributions: Distribution[]; // kept to stop premature deallocation

    static create(distributions: Distribution[], distributionSize: number, distributionsWidth: number): DistributionArea {
        const distributionsPtr = mallocInst(distributions.length * 8);

        for (let i = 0; i < distributions.length; i++) {
            setValue(distributionsPtr + i * 8, distributions[i].ptr, "*");
        }

        const area = new DistributionArea(distribution_area_create(distributionsPtr, distributionSize, distributionsWidth), distributions);
        distributionAreaRegistry.register(area, area.ptr, area);

        return area;
    }

    constructor(ptr: number, distributions: Distribution[]) {
        this.ptr = ptr;
        this.distributions = distributions;
    }

    free() {
        distributionAreaRegistry.unregister(this);
        distribution_area_free(this.ptr);
    }
}
