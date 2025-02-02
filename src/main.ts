import { init as initWrapper } from "./cwrapper";
import { Renderer } from "./render";

const canvasElement = document.querySelector<HTMLCanvasElement>("#canvas")!
const renderer = new Renderer(canvasElement);

async function init() {
    await Promise.all([
        renderer.load(),
        initWrapper()]);

    renderer.start();
}

init();