import { Vector2 } from "./vector2";

export class Camera {
    position: Vector2;
    zoom: number;

    constructor() {
        this.position = Vector2.zero();
        this.zoom = 1 / 64;
    }
}