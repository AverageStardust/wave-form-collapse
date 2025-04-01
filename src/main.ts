import { init as initWrapper } from "./cwrapper";
import { Distribution } from "./distribution";
import { Renderer } from "./render";
import { SimpleSuperposition } from "./superposition";
import { Tileset } from "./tileset";
import { World } from "./world";

const canvasElement = document.querySelector<HTMLCanvasElement>("#canvas")!
const renderer = new Renderer(canvasElement);

async function init() {
    await Promise.all([
        renderer.load(),
        initWrapper()]);

    const tileset = Tileset.create();
    const dirtEdge = 0, roadEdge = 1;
    const dirt = tileset.addTile(0, 0, dirtEdge);
    const road = tileset.addTile(8, 0, dirtEdge, roadEdge);

    const world = World.create(16, tileset);
    world.createChunk(0, 0);

    const distribution = Distribution.create(tileset);
    distribution.addTile(dirt, 5);
    distribution.addTile(road, 1);

    const superposition = SimpleSuperposition.create(world, distribution);
    superposition.collapse(0, 0, 16, 16);

    renderer.setWorld(world);

    renderer.start();
}

init();