let intlist_create: (inital_allocated_length: number) => number;
let intlist_push: (ptr: number, value: number) => null;
let intlist_free: (ptr: number) => null;

const listRegistry = new FinalizationRegistry((ptr: number) => {
    intlist_free(ptr);
});

export function init() {
    intlist_create = cwrap("intlist_create", "number", ["number"]);
    intlist_push = cwrap("intlist_push", null, ["number", "number"]);
    intlist_free = cwrap("intlist_free", null, ["number"]);
}


export class IntList {
    ptr: number;

    static create(initalAllocatedLength = 4) {
        const list = new IntList(intlist_create(initalAllocatedLength));
        listRegistry.register(list, list.ptr);
    }

    static adopt(ptr: number) {
        const list = new IntList(ptr);
        listRegistry.register(list, list.ptr);
        return list;
    }

    constructor(ptr: number) {
        this.ptr = ptr;
    }

    get length() {
        return getValue(this.ptr + 0, "i32")
    }

    get allocatedLength() {
        return getValue(this.ptr + 4, "i32")
    }

    at(index: number) {
        index = Math.trunc(index);
        if (index < 0) index = this.length - index;
        if (index >= this.length) throw RangeError("Out of bounds IntList access");

        const elementsPtr = getValue(this.ptr + 8, "i32*")
        return getValue(elementsPtr + index * 4, "i32");
    }

    push(value: number) {
        intlist_push(this.ptr, value);
    }

    free() {
        intlist_free(this.ptr);
    }
}