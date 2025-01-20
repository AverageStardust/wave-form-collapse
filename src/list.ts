import { free } from "./cwrapper";

export class IntList {
    structPtr: number;
    elementsPtr: number;
    length: number;

    constructor(pointer: number) {
        this.structPtr = pointer;
        this.elementsPtr = getValue(pointer, "i32*");
        this.length = getValue(pointer + 4, "i32");
    }

    free() {
        free(this.elementsPtr);
        free(this.structPtr);
    }
}