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
    const car0 = tileset.addTile(43, 0, 0, 0, 0, 0);
    const car90 = tileset.addTile(43, 5, 0, 0, 0, 0);
    const car180 = tileset.addTile(43, 3, 0, 0, 0, 0);
    const car270 = tileset.addTile(43, 6, 0, 0, 0, 0);

    world = World.create(16, tileset);
    world.createChunk(0, 0);
    world.set(0, 0, car0);
    world.set(1, 0, car90);
    world.set(2, 0, car180);
    world.set(3, 0, car270);
    renderer.setWorld(world);

    renderer.start();
}

init();