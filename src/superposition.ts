import { DistributionArea } from "./distribution";
import { World } from "./world";

let superposition_create: (world: number) => number;
let superposition_set_area: (superposition: number, area: number, x: number, y: number) => void;
let superposition_collapse: (superposition: number, u: number, v: number, width: number, height: number) => void;
let superposition_free: (superposition: number) => void;

const superpositionRegistry = new FinalizationRegistry((ptr: number) => {
    superposition_free(ptr);
});

export function init() {
    superposition_create = cwrap("distribution_create", "number", ["number"]);
    superposition_set_area = cwrap("distribution_create", null, ["number", "number", "number", "number"]);
    superposition_collapse = cwrap("distribution_create", null, ["number", "number", "number", "number", "number"]);
    superposition_free = cwrap("distribution_create", null, ["number"]);
}

export class Superposition {
    readonly ptr: number;

    static create(world: World): Superposition {
        const superposition = new Superposition(superposition_create(world.ptr));
        superpositionRegistry.register(superposition, superposition.ptr);
        return superposition;
    }

    constructor(ptr: number) {
        this.ptr = ptr;
    }

    setArea(area: DistributionArea, x: number, y: number) {
        superposition_set_area(this.ptr, area.ptr, x, y);
    }

    collapse(u: number, v: number, width: number, height: number) {
        superposition_collapse(this.ptr, u, v, width, height);
    }
}