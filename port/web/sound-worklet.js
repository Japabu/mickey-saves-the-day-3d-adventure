// sound-worklet.js - plays the game's sound ring (platform/host/web/sound.cpp): stereo
// float frames in the program's shared memory, silence when it runs dry. The page sends
// the memory (again after it grows) and where the shared fields are.
const SOUND_RING = 27, SOUND_FRAMES = 28, SOUND_WRITE = 29, SOUND_READ = 30;

class MickeySound extends AudioWorkletProcessor {
  constructor() {
    super();
    this.i32 = null;
    this.port.onmessage = (e) => {
      this.i32 = new Int32Array(e.data.buffer);
      this.f32 = new Float32Array(e.data.buffer);
      this.base = e.data.base >> 2;
    };
  }

  process(inputs, outputs) {
    const left = outputs[0][0], right = outputs[0][1] || left;
    left.fill(0);
    right.fill(0);
    if (!this.i32) return true;
    const b = this.base;
    const ring = Atomics.load(this.i32, b + SOUND_RING) >> 2;
    const frames = Atomics.load(this.i32, b + SOUND_FRAMES);
    if (!ring || ring + frames * 2 > this.f32.length) return true;  // not yet, or memory grew
    const written = Atomics.load(this.i32, b + SOUND_WRITE) >>> 0;
    const played = Atomics.load(this.i32, b + SOUND_READ) >>> 0;
    const n = Math.min(left.length, (written - played) >>> 0);
    for (let i = 0; i < n; i++) {
      const at = ((played + i) >>> 0) % frames;
      left[i] = this.f32[ring + at * 2];
      right[i] = this.f32[ring + at * 2 + 1];
    }
    Atomics.store(this.i32, b + SOUND_READ, (played + n) | 0);
    return true;
  }
}

registerProcessor('mickey-sound', MickeySound);
