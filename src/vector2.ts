export class Vector2 {
    x: number;
    y: number;

    static zero() {
        return new Vector2(0, 0);
    }

    static one() {
        return new Vector2(1, 1);
    }

    static up() {
        return new Vector2(0, 1);
    }

    static down() {
        return new Vector2(0, -1);
    }

    static right() {
        return new Vector2(1, 0);
    }

    static left() {
        return new Vector2(-1, 0);
    }

    static fromArray([x, y]: [number, number]): Vector2 {
        return new Vector2(x, y);
    }

    static fromObject({ x, y }: { x: number, y: number }): Vector2 {
        return new Vector2(x, y);
    }

    static fromAngle(angle: number, length = 1): Vector2 {
        return new Vector2(Math.cos(angle) * length, Math.sin(angle) * length);
    }

    constructor(x: number, y: number = x) {
        this.x = x;
        this.y = y;
    }

    get length(): number {
        return Math.hypot(this.x, this.y);
    }

    get lengthSq(): number {
        return this.x ** 2 + this.y ** 2;
    }

    get angle(): number {
        return Math.atan2(this.y, this.x);
    }

    get array(): [number, number] {
        return [this.x, this.y];
    }

    get object(): { x: number, y: number } {
        return { x: this.x, y: this.y };
    }

    get string(): string {
        return this.toString();
    }

    get clone(): Vector2 {
        return new Vector2(this.x, this.y);
    }

    toString(): string {
        return `(${this.x}, ${this.y})`
    }

    toFixedString(precision: number = 2): string {
        return `(${this.x.toFixed(precision)}, ${this.y.toFixed(precision)})`;
    }

    add(vector: Vector2): Vector2 {
        this.x += vector.x;
        this.y += vector.y;
        return this;
    }

    sub(vector: Vector2): Vector2 {
        this.x -= vector.x;
        this.y -= vector.y;
        return this;
    }

    mult(vector: Vector2): Vector2 {
        this.x *= vector.x;
        this.y *= vector.y;
        return this;
    }

    div(vector: Vector2): Vector2 {
        this.x /= vector.x;
        this.y /= vector.y;
        return this;
    }

    scale(scalar: number): Vector2 {
        this.x *= scalar;
        this.y *= scalar;
        return this;
    }

    inverseScale(scalar: number): Vector2 {
        const inverseScalar = 1 / scalar;
        this.x *= inverseScalar;
        this.y *= inverseScalar;
        return this;
    }

    mix(vector: Vector2, amount: number = 0.5): Vector2 {
        this.x += (vector.x - this.x) * amount;
        this.y += (vector.y - this.y) * amount;
        return this;
    }

    norm(targetLength: number = 1): Vector2 {
        const length = this.length;
        if (this.length === 0.0) return this;
        const inverseLength = targetLength / length;
        this.x *= inverseLength;
        this.y *= inverseLength;
        return this;
    }

    limit(maxLength: number = 1): Vector2 {
        const newLength = Math.min(this.length, maxLength);
        this.norm(newLength);
        return this;
    }

    round(): Vector2 {
        this.x = Math.round(this.x);
        this.y = Math.round(this.y);
        return this;
    }

    dot(vector: Vector2): number {
        return this.x * vector.x + this.y * vector.y;
    }

    cross(vector: Vector2): number {
        return this.x * vector.y - this.y * vector.x;
    }

    reflect(normal: Vector2): Vector2 {
        const scalar = -2 * (this.x * normal.x + this.y * normal.y);
        this.x -= normal.x * scalar;
        this.y -= normal.y * scalar;
        return this;
    }

    projectScalar(vector: Vector2) {
        return (this.x * vector.x + this.y * vector.y) / vector.length;
    }

    project(vector: Vector2): Vector2 {
        const inverseVectorLength = 1 / vector.length;
        const scalar = (this.x * vector.x + this.y * vector.y) * inverseVectorLength * inverseVectorLength;
        this.x = vector.x * scalar;
        this.y = vector.y * scalar;
        return this;
    }

    rejectScalar(vector: Vector2): number {
        return (this.y * vector.x - this.x * vector.y) / vector.length;
    }

    reject(vector: Vector2): Vector2 {
        const scalar = (this.y * vector.x - this.x * vector.y) / (vector.x * vector.x + vector.y * vector.y);
        this.x = -vector.y * scalar;
        this.y = vector.x * scalar;
        return this;
    }

    angleBetween(vector: Vector2): number {
        const cosAngle = (this.x * vector.x + this.y * vector.y) / (this.length * vector.length);
        return Math.acos(cosAngle);
    }

    rotate(angle: number): Vector2 {
        const sinAngle = Math.sin(angle);
        const cosAngle = Math.cos(angle);
        const oldX = this.x;
        this.x = cosAngle * this.x + -sinAngle * this.y;
        this.x = sinAngle * oldX + cosAngle * this.y;

        return this;
    }

    rotateTo(angle: number): Vector2 {
        const length = this.length;
        this.x = Math.cos(angle) * length;
        this.y = Math.sin(angle) * length;
        return this;
    }

    dist(vector: Vector2): number {
        return Math.hypot(vector.x - this.x, vector.y - this.y);
    }

    distSq(vector: Vector2): number {
        return ((vector.x - this.x) ** 2 + (vector.y - this.y) ** 2);
    }

    equalTo(vector: Vector2): boolean {
        return this.x === vector.x && this.y === vector.y;
    }

    equalToSoft(vector: Vector2, epsilon: number = 1e-2): boolean {
        const differenceSum = Math.abs(this.x - vector.x) + Math.abs(this.y - vector.y);
        return differenceSum <= epsilon;
    }

    clampToZero(epsilon = 1e-2): Vector2 {
        if (this.equalToSoft(new Vector2(0, 0), epsilon))
            this.x = this.y = 0;

        return this;
    }
}