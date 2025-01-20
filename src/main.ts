import { getUndisplayedChunks } from "./cwrapper";

document.querySelector<HTMLDivElement>('#app')!.innerText = "hello world";

console.log(getUndisplayedChunks);