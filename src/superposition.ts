import { Distribution, DistributionArea } from "./distribution";
import { DistributionWorld, World } from "./world";

let superposition_create: (world: number) => number;
let superposition_select_distribution_area: (superposition: number, x: number, y: number, area: number) => void;
let superposition_select_collapse_area: (superposition: number, u: number, v: number, width: number, height: number) => void;
let superposition_collapse_tiles: (superposition: number, amount: number) => number;
let superposition_free: (superposition: number) => void;

const superpositionRegistry = new FinalizationRegistry((ptr: number) => {
    superposition_free(ptr);
});

export function init() {
    superposition_create = cwrap("superposition_create", "number", ["number"]);
    superposition_select_distribution_area = cwrap("superposition_select_distribution_area", null, ["number", "number", "number", "number"]);
    superposition_select_collapse_area = cwrap("superposition_select_collapse_area", null, ["number", "number", "number", "number", "number"]);
    superposition_collapse_tiles = cwrap("superposition_collapse_tiles", "number", ["number", "number"]);
    superposition_free = cwrap("superposition_free", null, ["number"]);
}

class SuperpositionAbstract {
    readonly ptr: number;
    readonly destinationWorld: World; // always kept to stop premature deallocation, also used by FractalSuperposition

    constructor(ptr: number, destinationWorld: World) {
        this.ptr = ptr;
        this.destinationWorld = destinationWorld;
    }

    selectCollapseArea(u: number, v: number, width: number, height: number) {
        superposition_select_collapse_area(this.ptr, u, v, width, height);
    }

    collapse(amount: number) {
        superposition_collapse_tiles(this.ptr, amount);
    }

    free() {
        superpositionRegistry.unregister(this);
        superposition_free(this.ptr);
    }
}

export class SimpleSuperposition extends SuperpositionAbstract {
    readonly area: DistributionArea; // kept to stop premature deallocation

    static create(world: World, distribution: Distribution) {
        const area = distribution.getArea();
        const superposition = new SimpleSuperposition(superposition_create(world.ptr), world, area);

        superposition_select_distribution_area(superposition.ptr, 0, 0, area.ptr);

        superpositionRegistry.register(superposition, superposition.ptr, superposition);
        return superposition;
    }

    constructor(ptr: number, destinationWorld: World, area: DistributionArea) {
        super(ptr, destinationWorld);
        this.area = area;
    }
}

export class FractalSuperposition extends SuperpositionAbstract {
    readonly sourceWorld: DistributionWorld;

    static create(destinationWorld: World, sourceWorld: DistributionWorld) {
        const superposition = new FractalSuperposition(superposition_create(destinationWorld.ptr), destinationWorld, sourceWorld);
        if (destinationWorld.chunkSize % sourceWorld.distributionSize !== 0)
            throw Error("Can't build fractal superposition, source distribution tiles and destination chunks are unaligned.");
        superpositionRegistry.register(superposition, superposition.ptr, superposition);
        return superposition;
    }

    constructor(ptr: number, destinationWorld: World, sourceWorld: DistributionWorld) {
        super(ptr, destinationWorld);
        this.sourceWorld = sourceWorld;
    }

    selectChunk(x: number, y: number) {
        const chunkSize = this.destinationWorld.chunkSize;
        const area = this.sourceWorld.getArea(x * chunkSize, y * chunkSize, chunkSize);
        superposition_select_distribution_area(this.ptr, x * chunkSize, y * chunkSize, area.ptr);
    }
}