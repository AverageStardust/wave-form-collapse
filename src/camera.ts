import { Vector2 } from "./vector2";

export class Camera {
    position: Vector2;
    size: number;

    constructor() {
        this.position = Vector2.zero();
        this.size = 16;
    }
}