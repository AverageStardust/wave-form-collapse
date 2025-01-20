import { IntList } from "./list";

export const free = cwrap("_free", null, ["number"]);
export const getChunkSize = cwrap("get_chunk_size", "number", []);

const getUndisplayedChunksRaw = cwrap("get_undisplayed_chunks", "number", ["number", "number", "number", "number"]);

export function getUndisplayedChunks(x: number, y: number, width: number, height: number) {
    return new IntList(getUndisplayedChunksRaw(x, y, width, height));
}

export const readChunkTiles = cwrap("get_undisplayed_chunks", "number", ["number", "number"]);