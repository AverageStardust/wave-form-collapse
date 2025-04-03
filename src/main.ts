import { init as initWrapper } from "./cwrapper";
import { Distribution } from "./distribution";
import { getMemoryUsage } from "./meminst";
import { Renderer } from "./render";
import { SimpleSuperposition } from "./superposition";
import { Tileset } from "./tileset";
import { Vector2 } from "./vector2";
import { World } from "./world";

const canvasElement = document.querySelector<HTMLCanvasElement>("#canvas")!
const renderer = new Renderer(canvasElement);

async function init() {
    await Promise.all([
        renderer.load(),
        initWrapper()]);

    const tileset = Tileset.create(8, 8);
    const dirtEdge = 0, roadEdge = 1;
    const dirt = tileset.addTile(0, 0, dirtEdge);
    const roadVertical = tileset.addTile(8, 0, dirtEdge, roadEdge);
    const roadTop = tileset.addTile(12, 0, dirtEdge, dirtEdge, dirtEdge, roadEdge);
    const roadBottom = tileset.addTile(12, 3, dirtEdge, roadEdge, dirtEdge, dirtEdge);

    const world = World.create(16, tileset);
    world.createChunk(0, 0);

    const distribution = Distribution.create(tileset);
    distribution.addTile(dirt, 10);
    distribution.addTile(roadVertical, 2);
    distribution.addTile(roadTop, 1);
    distribution.addTile(roadBottom, 1);

    const superposition = SimpleSuperposition.create(world, distribution);
    superposition.selectCollapseArea(0, 0, 16, 16);
    superposition.collapse(256);

    renderer.setWorld(world);
    renderer.camera.position.add(new Vector2(8, 8));
    renderer.start();
}

init();