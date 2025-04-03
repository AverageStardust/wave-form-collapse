let list32_create: (inital_allocated_length: number) => number;
let list64_create: (inital_allocated_length: number) => number;
let list32_push: (ptr: number, value: number) => null;
let list64_push: (ptr: number, value: number) => null;
let list_free: (ptr: number) => null;

const listRegistry = new FinalizationRegistry((ptr: number) => {
    list_free(ptr);
});

export function init() {
    list32_create = cwrap("list32_create", "number", ["number"]);
    list64_create = cwrap("list64_create", "number", ["number"]);
    list32_push = cwrap("list_push32", null, ["number", "number"]);
    list64_push = cwrap("list_push64", null, ["number", "number"]);
    list_free = cwrap("list_free", null, ["number"]);
}


export class List {
    readonly ptr: number;
    readonly elementSize: 4 | 8;

    static create32(initalAllocatedLength: number) {
        const ptr = list32_create(initalAllocatedLength);
        const list = new List(ptr, 4)
        listRegistry.register(list, list.ptr);
    }

    static create64(initalAllocatedLength: number) {
        const ptr = list64_create(initalAllocatedLength);
        const list = new List(ptr, 8)
        listRegistry.register(list, list.ptr);
    }

    static adopt32(ptr: number) {
        const list = new List(ptr, 4);
        listRegistry.register(list, list.ptr);
        return list;
    }

    static adopt64(ptr: number) {
        const list = new List(ptr, 8);
        listRegistry.register(list, list.ptr);
        return list;
    }

    constructor(ptr: number, elementSize: 4 | 8) {
        this.ptr = ptr;
        this.elementSize = elementSize;
    }

    get length() {
        return getValue(this.ptr + 0, "i32");
    }

    get allocatedLength() {
        return getValue(this.ptr + 4, "i32");
    }

    at(index: number) {
        index = Math.trunc(index);
        if (index < 0) index = this.length - index;
        if (index >= this.length) throw RangeError("Out of bounds list access");

        const elementsPtr = getValue(this.ptr + 8, "i32*");
        if (this.elementSize == 4) {
            return getValue(elementsPtr + index * 4, "i32");
        } else {
            let lower = getValue(elementsPtr + index * 8, "i32");
            let upper = getValue(elementsPtr + index * 8 + 4, "i32");
            if (upper < 0)
                lower = ~lower + 1;

            return upper * (2 ** 32) + lower;
        }
    }

    push(value: number) {
        if (this.elementSize == 4) {
            list32_push(this.ptr, value);
        } else {
            list64_push(this.ptr, value);
        }
    }

    free() {
        list_free(this.ptr);
    }
}