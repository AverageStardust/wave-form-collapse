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

    at(index: number) {
        index = Math.trunc(index);
        if (index < 0) index = this.length - index;
        if (index >= this.length) throw RangeError("Out of bounds IntList access");

        return getValue(this.elementsPtr + index * 4, "i32");
    }

    free() {
        free(this.elementsPtr);
        free(this.structPtr);
    }
}