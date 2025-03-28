import { init as initWrapper } from "./cwrapper";
import { Distribution } from "./distribution";
import { Renderer } from "./render";
import { Tileset } from "./tileset";
import { World } from "./world";

const canvasElement = document.querySelector<HTMLCanvasElement>("#canvas")!
const renderer = new Renderer(canvasElement);
let world: World;

async function init() {
    await Promise.all([
        renderer.load(),
        initWrapper()]);

    const tileset = Tileset.create();
    const dirtEdge = 0, roadEdge = 1;
    const dirt = tileset.addTile(0, 0, dirtEdge);
    const road = tileset.addTile(8, 0, dirtEdge, roadEdge);

    const distribution = Distribution.create(tileset);
    distribution.addTile(dirt, 5);
    distribution.addTile(road, 1);

    world = World.create(16, tileset);
    world.createChunk(0, 0);
    world.set(0, 0, dirt);
    world.set(1, 0, dirt);
    world.set(2, 0, road);
    world.set(3, 0, dirt);
    renderer.setWorld(world);

    renderer.start();
}

init();