import { init as initWrapper } from "./cwrapper";
import { Renderer } from "./render";
import { World } from "./world";

const canvasElement = document.querySelector<HTMLCanvasElement>("#canvas")!
const renderer = new Renderer(canvasElement);
let world: World;

async function init() {
    await Promise.all([
        renderer.load(),
        initWrapper()]);

    world = World.create(16);
    world.createChunk(0, 0);
    world.set(0, 0, 32);
    renderer.setWorld(world);

    renderer.start();
}

init();