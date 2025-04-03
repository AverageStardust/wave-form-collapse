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

    const tileset = Tileset.create(8, 16);
    const dirtEdge = 0, roadEdge = 1;
    const dirt = tileset.addTile(0, 0, dirtEdge);
    const roadVertical = tileset.addTile(8, 0, dirtEdge, roadEdge);
    const roadHoizontal = tileset.addTile(8, 4, roadEdge, dirtEdge);
    const roadTop = tileset.addTile(12, 0, dirtEdge, dirtEdge, dirtEdge, roadEdge);
    const roadBottom = tileset.addTile(12, 3, dirtEdge, roadEdge, dirtEdge, dirtEdge);
    const roadRight = tileset.addTile(12, 4, dirtEdge, dirtEdge, roadEdge, dirtEdge);
    const roadLeft = tileset.addTile(12, 7, roadEdge, dirtEdge, dirtEdge, dirtEdge);
    const roadIntersection = tileset.addTile(11, 0, roadEdge, roadEdge, roadEdge, roadEdge);

    const world = World.create(16, tileset);
    world.createChunk(0, 0);

    const distribution = Distribution.create(tileset);
    distribution.addTile(dirt, 50);
    distribution.addTile(roadVertical, 8);
    distribution.addTile(roadHoizontal, 8);
    distribution.addTile(roadTop, 1);
    distribution.addTile(roadBottom, 1);
    distribution.addTile(roadRight, 1);
    distribution.addTile(roadLeft, 1);
    distribution.addTile(roadIntersection, 1);

    const superposition = SimpleSuperposition.create(world, distribution);
    superposition.selectCollapseArea(0, 0, 16, 16);

    renderer.setWorld(world);
    renderer.camera.position.add(new Vector2(8, 8));
    renderer.start();

    renderer.addEventListener("frame", () => {
        superposition.collapse(1);
    });
}

init();