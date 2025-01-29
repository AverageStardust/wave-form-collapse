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

// const intList = getUndisplayedChunks(0, 0, 2, 2);
// for (let i = 0; i < intList.length; i += 2) {
//     const x = intList.at(i), y = intList.at(i + 1);
//     //getChunkTiles(x, y, true);
// }
// intList.free();