import { Distribution } from "./distribution";
import { DistributionWorld, World } from "./world";

let superposition_create: (world: number) => number;
let superposition_select_area: (superposition: number, x: number, y: number, area: number) => void;
let superposition_collapse: (superposition: number, u: number, v: number, width: number, height: number) => void;
let superposition_free: (superposition: number) => void;

const superpositionRegistry = new FinalizationRegistry((ptr: number) => {
    superposition_free(ptr);
});

export function init() {
    superposition_create = cwrap("superposition_create", "number", ["number"]);
    superposition_select_area = cwrap("superposition_select_area", null, ["number", "number", "number", "number"]);
    superposition_collapse = cwrap("superposition_collapse", null, ["number", "number", "number", "number", "number"]);
    superposition_free = cwrap("superposition_free", null, ["number"]);
}

class SuperpositionAbstract {
    readonly ptr: number;

    constructor(ptr: number) {
        this.ptr = ptr;
    }

    collapse(u: number, v: number, width: number, height: number) {
        superposition_collapse(this.ptr, u, v, width, height);
    }
}

export class SimpleSuperposition extends SuperpositionAbstract {
    static create(world: World, distribution: Distribution) {
        const superposition = new SimpleSuperposition(superposition_create(world.ptr));

        const area = distribution.getArea();
        superposition_select_area(superposition.ptr, 0, 0, area.ptr);

        superpositionRegistry.register(superposition, superposition.ptr);
        return superposition;
    }
}

export class FractalSuperposition extends SuperpositionAbstract {
    destinationWorld: World;
    sourceWorld: DistributionWorld;

    static create(destinationWorld: World, sourceWorld: DistributionWorld) {
        const superposition = new FractalSuperposition(superposition_create(destinationWorld.ptr), destinationWorld, sourceWorld);
        if (destinationWorld.chunkSize % sourceWorld.distributionSize !== 0)
            throw Error("Can't build fractal superposition, source distribution tiles and destination chunks are unaligned.");
        superpositionRegistry.register(superposition, superposition.ptr);
        return superposition;
    }

    constructor(ptr: number, destinationWorld: World, sourceWorld: DistributionWorld) {
        super(ptr);
        this.destinationWorld = destinationWorld;
        this.sourceWorld = sourceWorld;
    }

    selectChunk(x: number, y: number) {
        const chunkSize = this.destinationWorld.chunkSize;
        const area = this.sourceWorld.getArea(x * chunkSize, y * chunkSize, chunkSize);
        superposition_select_area(this.ptr, x * chunkSize, y * chunkSize, area.ptr);
    }
}