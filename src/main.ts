import { init as initWrapper } from "./cwrapper";
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
    tileset.addTile("car", 43, 0, 0, 0, 0);

    world = World.create(16, tileset);
    world.createChunk(0, 0);
    world.set(0, 0, "car");
    renderer.setWorld(world);

    renderer.start();
}

init();