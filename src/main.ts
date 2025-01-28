import { getChunkTiles, getUndisplayedChunks } from "./cwrapper";

const appElement = document.querySelector<HTMLDivElement>('#app')!
appElement.innerText = "hello world\n";

const intList = getUndisplayedChunks(0, 0, 2, 2);
for (let i = 0; i < intList.length; i += 2) {
    const x = intList.at(i), y = intList.at(i + 1);
    appElement.append(String(getChunkTiles(x, y)));
}
intList.free();