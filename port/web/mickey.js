// mickey.js - the page around the game (the web host's other half, see
// port/platform/host/web/shared.h): the ISO is copied once into the site's private
// storage (copy-worker.js), saved games are exported and imported there, then Play starts
// the game (Steamboat.js, on a worker). This
// thread draws its frames with WebGL, turns DOM events into its event ring and plays its
// sound ring through an AudioWorklet (sound-worklet.js).
'use strict';

// The shared fields (shared.h: enum Field), the event ring and the event types
// (host.h: Event::Type).
const F = {
  MAGIC: 0, FRAME_SEQ: 1, FRAME_FRONT: 2, FRAME_READING: 3, FRAME_PTR: 4, FRAME_W: 7, FRAME_H: 10,
  FRAME_SCALE: 13, SMOOTH: 16, WINDOW_SHOWN: 17, CURSOR_VISIBLE: 18, FULLSCREEN_WANT: 19,
  FULLSCREEN_IS: 20, PAGE_VISIBLE: 21, MOUSE_X: 22, MOUSE_Y: 23, EV_HEAD: 24, EV_TAIL: 25,
  SOUND_RATE: 26, SOUND_RING: 27, SOUND_FRAMES: 28, SOUND_WRITE: 29, SOUND_READ: 30, GAME_W: 31,
  GAME_H: 32, FIELDS: 40,
};
const EVENTS = 256, EVENT_INTS = 18;
const E = { TYPE: 0, X: 1, Y: 2, BUTTON: 3, CLICKS: 4, WHEEL_MILLI: 5, VK: 6, SIDE_VK: 7, SCANCODE: 8, FLAGS: 9, TEXT: 10 };
const T = { Close: 0, FocusIn: 1, FocusOut: 2, Minimized: 3, Restored: 4, Exposed: 5, MouseMove: 6, MouseDown: 7,
            MouseUp: 8, Wheel: 9, KeyDown: 10, KeyUp: 11, Text: 12 };

// KeyboardEvent.code -> the Windows virtual-key code (host.h: Key), and for the
// modifiers their left/right code.
const VK = {
  Backspace: 0x08, Tab: 0x09, Enter: 0x0D, NumpadEnter: 0x0D, ShiftLeft: 0x10, ShiftRight: 0x10,
  ControlLeft: 0x11, ControlRight: 0x11, AltLeft: 0x12, AltRight: 0x12, Pause: 0x13, CapsLock: 0x14,
  Escape: 0x1B, Space: 0x20, PageUp: 0x21, PageDown: 0x22, End: 0x23, Home: 0x24, ArrowLeft: 0x25,
  ArrowUp: 0x26, ArrowRight: 0x27, ArrowDown: 0x28, PrintScreen: 0x2C, Insert: 0x2D, Delete: 0x2E,
  MetaLeft: 0x5B, MetaRight: 0x5C, NumpadMultiply: 0x6A, NumpadAdd: 0x6B, NumpadSubtract: 0x6D,
  NumpadDecimal: 0x6E, NumpadDivide: 0x6F, NumLock: 0x90, ScrollLock: 0x91, Semicolon: 0xBA, Equal: 0xBB,
  Comma: 0xBC, Minus: 0xBD, Period: 0xBE, Slash: 0xBF, Backquote: 0xC0, BracketLeft: 0xDB, Backslash: 0xDC,
  BracketRight: 0xDD, Quote: 0xDE,
};
for (let i = 0; i < 26; i++) VK['Key' + String.fromCharCode(65 + i)] = 65 + i;
for (let i = 0; i < 10; i++) { VK['Digit' + i] = 48 + i; VK['Numpad' + i] = 0x60 + i; }
for (let i = 1; i <= 12; i++) VK['F' + i] = 0x6F + i;
const SIDE_VK = { ShiftLeft: 0xA0, ShiftRight: 0xA1, ControlLeft: 0xA2, ControlRight: 0xA3, AltLeft: 0xA4, AltRight: 0xA5 };
const IS_MAC = /Mac|iPhone|iPad/.test(navigator.platform);

const $ = (id) => document.getElementById(id);

// ---- The CD in the site's storage

async function storedCd() {
  try {
    const root = await navigator.storage.getDirectory();
    const ok = await (await root.getFileHandle('cd.ok')).getFile();
    return JSON.parse(await ok.text());
  } catch (_) {
    return null;
  }
}

// An ISO 9660 image (CD001 at 32 KB + 1), and its volume label.
async function isoLabel(file) {
  const d = new Uint8Array(await file.slice(0x8000, 0x8048).arrayBuffer());
  if (String.fromCharCode(...d.slice(1, 6)) !== 'CD001') return null;
  return String.fromCharCode(...d.slice(40, 72)).trim();
}

function copyCd(file) {
  return new Promise((resolve, reject) => {
    const worker = new Worker('copy-worker.js');
    const bar = $('progress');
    bar.hidden = false;
    worker.onmessage = (e) => {
      if (e.data.progress !== undefined) bar.value = e.data.progress;
      if (e.data.done) { worker.terminate(); bar.hidden = true; resolve(); }
      if (e.data.error) { worker.terminate(); bar.hidden = true; reject(new Error(e.data.error)); }
    };
    worker.postMessage({ file });
  });
}

function say(text) { $('message').textContent = text; }

async function showStart() {
  const cd = await storedCd();
  $('play-row').hidden = !cd;
  $('choose-row').hidden = !!cd;
  $('saves').hidden = !cd;
  if (cd) await showSaves();
  say(cd ? `Ready: ${cd.name} (${(cd.size / 1048576).toFixed(0)} MB).` : '');
}

async function chosen(file) {
  const label = await isoLabel(file);
  if (label === null) { say(`${file.name} is not a CD image (.iso).`); return; }
  if (!/^mick/i.test(label)) say(`${file.name} is labelled "${label}", not the game's CD; copying it anyway...`);
  else say(`Copying ${file.name} into the browser's storage...`);
  try {
    if (navigator.storage.persist) await navigator.storage.persist();
    await copyCd(file);
  } catch (err) {
    say(`Copying failed: ${err.message}`);
    return;
  }
  await showStart();
}

// ---- Saved games: the game's eight slots are savedgame<N>.ini in its data directory
// (/opfs/data, host/web/host.cpp) with their index in SGDB.ini: a section [game N data]
// (icon = N, the slot's picture, which is also the file's number; scene = where it was
// saved) per slot, and [used games] games = the slot numbers. The game finds its files
// whatever their case (win32/vfs.cpp), so these are looked up the same way. An exported
// game is one JSON file: the slot's ini text and the scene.

const SAVE_FORMAT = 'mickey-saves-the-day/saved-game', SLOTS = 8, CRLF = '\r\n';
// The slots' pictures in the game, as emoji: Mickey's shorts, his glove, his shoe,
// Minnie's bow, Goofy's hat, Donald's cap, Pluto's bone, a daisy.
const SLOT_ICONS = [
  ['🩳', "Mickey's shorts"], ['🧤', "Mickey's glove"], ['👞', "Mickey's shoe"], ['🎀', "Minnie's bow"],
  ['🎩', "Goofy's hat"], ['🧢', "Donald's cap"], ['🦴', "Pluto's bone"], ['🌼', 'Daisy'],
];

// The game's directory of ini files, created if asked (as the game would name it).
async function binariesDir(create) {
  let dir = await navigator.storage.getDirectory();
  for (const name of ['data', 'setup', 'hd', 'binaries']) {
    let found = null;
    for await (const [n, h] of dir.entries())
      if (h.kind === 'directory' && n.toLowerCase() === name) found = h;
    if (!found && !create) return null;
    dir = found || await dir.getDirectoryHandle(name, { create: true });
  }
  return dir;
}

async function findFile(dir, name) {
  for await (const [n, h] of dir.entries())
    if (h.kind === 'file' && n.toLowerCase() === name.toLowerCase()) return h;
  return null;
}

async function readText(dir, name) {
  const h = dir && await findFile(dir, name);
  return h ? (await h.getFile()).text() : null;
}

async function writeText(dir, name, text) {
  const h = await findFile(dir, name) || await dir.getFileHandle(name, { create: true });
  const w = await h.createWritable();
  await w.write(text);
  await w.close();
}

// An ini text as its sections in order: { name, lines } (the lines before the first
// section have the name null).
function iniSections(text) {
  const sections = [{ name: null, lines: [] }];
  for (const line of (text || '').split(/\r?\n/)) {
    const m = /^\s*\[(.*)\]\s*$/.exec(line);
    if (m) sections.push({ name: m[1].trim(), lines: [] });
    else sections[sections.length - 1].lines.push(line);
  }
  return sections;
}

function iniValue(lines, key) {
  for (const line of lines) {
    const eq = line.indexOf('=');
    if (eq > 0 && line.slice(0, eq).trim().toLowerCase() === key) return line.slice(eq + 1).trim();
  }
  return null;
}

// The used slots of SGDB.ini: [{ slot, scene }] by slot.
function sgdbSlots(text) {
  const sections = iniSections(text);
  const used = sections.find((s) => s.name?.toLowerCase() === 'used games');
  const slots = [];
  for (const token of (used && iniValue(used.lines, 'games') || '').split(',')) {
    const slot = parseInt(token, 10);
    if (!(slot >= 1 && slot <= SLOTS) || slots.some((s) => s.slot === slot)) continue;
    const data = sections.find((s) => s.name?.toLowerCase() === `game ${slot} data`);
    slots.push({ slot, scene: data && iniValue(data.lines, 'scene') || '' });
  }
  return slots.sort((a, b) => a.slot - b.slot);
}

// SGDB.ini with the given slots, its other sections as they were.
function sgdbText(old, slots) {
  const keep = iniSections(old).filter((s) => s.name !== null && s.name.toLowerCase() !== 'used games' &&
                                              !/^game \d+ data$/i.test(s.name));
  let out = '';
  for (const s of slots) out += `[game ${s.slot} data]${CRLF}icon = ${s.slot}${CRLF}scene = ${s.scene}${CRLF}${CRLF}`;
  out += `[used games]${CRLF}games = ${slots.map((s) => s.slot).join(',')}${CRLF}`;
  for (const s of keep) {
    while (s.lines.length && s.lines[s.lines.length - 1].trim() === '') s.lines.pop();
    out += `${CRLF}[${s.name}]${CRLF}${s.lines.map((l) => l + CRLF).join('')}`;
  }
  return out;
}

function sayAboutSaves(text) { $('saves-message').textContent = text; }

async function savedGameFile(dir, slot) {
  const h = await findFile(dir, `savedgame${slot}.ini`);
  if (!h) return null;
  const file = await h.getFile();
  const scene = sgdbSlots(await readText(dir, 'SGDB.ini')).find((s) => s.slot === slot)?.scene || '';
  const json = JSON.stringify({ format: SAVE_FORMAT, version: 1, scene, saved: file.lastModified,
                                ini: await file.text() });
  const day = new Date(file.lastModified).toISOString().slice(0, 10);
  return new File([json], `Mickey saved game ${slot}${scene ? ' ' + scene : ''} ${day}.json`,
                  { type: 'application/json' });
}

async function exportSave(slot, share) {
  try {
    const file = await savedGameFile(await binariesDir(false), slot);
    if (!file) { sayAboutSaves(`${SLOT_ICONS[slot - 1][1]} has no saved game file.`); return; }
    if (share) {
      await navigator.share({ files: [file], title: 'Mickey Saves the Day: a saved game' });
      return;
    }
    const a = document.createElement('a');
    a.href = URL.createObjectURL(file);
    a.download = file.name;
    a.click();
    setTimeout(() => URL.revokeObjectURL(a.href), 10000);
  } catch (err) {
    if (err.name !== 'AbortError') sayAboutSaves(`Exporting failed: ${err.message}`);
  }
}

async function importSave(file) {
  try {
    if (file.size > 4 << 20) throw new Error(`${file.name} is too large for a saved game.`);
    let save;
    try { save = JSON.parse(await file.text()); } catch (_) { save = null; }
    if (!save || save.format !== SAVE_FORMAT || typeof save.ini !== 'string')
      throw new Error(`${file.name} is not a saved game exported from this page.`);
    const names = iniSections(save.ini).map((s) => s.name?.toLowerCase());
    if (!names.includes('game') || !names.includes('inventory'))
      throw new Error(`${file.name} does not hold a saved game.`);
    const scene = /^[A-Za-z]{2}$/.test(save.scene) ? save.scene.toUpperCase() : 'MX';
    const dir = await binariesDir(true);
    const old = await readText(dir, 'SGDB.ini');
    const slots = sgdbSlots(old);
    let slot = 1;
    while (slot <= SLOTS && slots.some((s) => s.slot === slot)) slot++;
    if (slot > SLOTS) throw new Error('All eight slots are used: erase one in the game first (Start, then a used picture).');
    await writeText(dir, `savedgame${slot}.ini`, save.ini);
    slots.push({ slot, scene });
    slots.sort((a, b) => a.slot - b.slot);
    await writeText(dir, 'SGDB.ini', sgdbText(old, slots));
    await showSaves();
    sayAboutSaves(`Imported as ${SLOT_ICONS[slot - 1][0]} ${SLOT_ICONS[slot - 1][1]}: in the game, Load shows it.`);
  } catch (err) {
    sayAboutSaves(err.message);
  }
}

async function showSaves() {
  const list = $('saves-list');
  list.replaceChildren();
  let slots = [];
  try {
    const dir = await binariesDir(false);
    slots = dir ? sgdbSlots(await readText(dir, 'SGDB.ini')) : [];
    // last used: when the game last wrote the slot's file (an imported one: when it came)
    for (const s of slots) s.used = (await (await findFile(dir, `savedgame${s.slot}.ini`))?.getFile())?.lastModified || 0;
    slots.sort((a, b) => b.used - a.used);
  } catch (err) {
    sayAboutSaves(`The saved games cannot be read: ${err.message}`);
  }
  const canShare = !!navigator.canShare?.({ files: [new File([''], 'x.json', { type: 'application/json' })] });
  const when = new Intl.DateTimeFormat(undefined, { dateStyle: 'medium', timeStyle: 'short' });
  for (const { slot, scene, used } of slots) {
    const li = document.createElement('li');
    const [emoji, picture] = SLOT_ICONS[slot - 1];
    const icon = document.createElement('b');
    icon.className = 'slot-icon';
    icon.textContent = emoji;
    icon.title = picture;
    li.append(icon);
    const label = document.createElement('span');
    label.textContent = `${picture}${scene ? ` (saved in ${scene})` : ''}${used ? `, last played ${when.format(used)}` : ''}`;
    li.append(label);
    const add = (text, share) => {
      const b = document.createElement('button');
      b.className = 'secondary';
      b.textContent = text;
      b.addEventListener('click', () => exportSave(slot, share));
      li.append(b);
    };
    add('Export', false);
    if (canShare) add('Share', true);
    list.append(li);
  }
  list.hidden = !slots.length;
  sayAboutSaves(slots.length ? '' : 'No saved games yet.');
}

// ---- The game

let mod, i32, u8, base, mem;

function heap() {
  // the program's memory grows; the views follow its buffer
  if (!i32 || i32.buffer !== mem.buffer) {
    i32 = new Int32Array(mem.buffer);
    u8 = new Uint8Array(mem.buffer);
    if (sound) sound.port.postMessage({ buffer: mem.buffer, base: base * 4 });
  }
}
const get = (f) => { heap(); return Atomics.load(i32, base + f); };
const set = (f, v) => { heap(); Atomics.store(i32, base + f, v); };

function push(type, fields = {}, text = '') {
  heap();
  const head = Atomics.load(i32, base + F.EV_HEAD), tail = Atomics.load(i32, base + F.EV_TAIL);
  if (head - tail >= EVENTS) return;  // the game is not reading: drop
  const at = base + F.FIELDS + ((head >>> 0) % EVENTS) * EVENT_INTS;
  i32.fill(0, at, at + EVENT_INTS);
  i32[at + E.TYPE] = type;
  for (const [k, v] of Object.entries(fields)) i32[at + E[k]] = v;
  const bytes = new TextEncoder().encode(text).slice(0, 31);
  u8.set(bytes, (at + E.TEXT) * 4);
  Atomics.store(i32, base + F.EV_HEAD, head + 1);
  Atomics.notify(i32, base + F.EV_HEAD);
}

// ---- Drawing: the newest frame, letterboxed; the pixels are 0xAARRGGBB, so B,G,R,A in
// memory, which the shader puts in order.
const canvas = $('screen');
let gl, tex, seen = -1, pixels = new Uint8Array(0), box = { x: 0, y: 0, w: 1, h: 1 };

function setupGl() {
  gl = canvas.getContext('webgl2', { antialias: false, alpha: false });
  const vs = `#version 300 es
    out vec2 uv; void main() { vec2 p = vec2(gl_VertexID & 1, gl_VertexID >> 1);
    uv = vec2(p.x, 1.0 - p.y); gl_Position = vec4(p * 2.0 - 1.0, 0, 1); }`;
  const fs = `#version 300 es
    precision mediump float; in vec2 uv; uniform sampler2D t; out vec4 c;
    void main() { c = vec4(texture(t, uv).bgr, 1); }`;
  const prog = gl.createProgram();
  for (const [type, src] of [[gl.VERTEX_SHADER, vs], [gl.FRAGMENT_SHADER, fs]]) {
    const s = gl.createShader(type);
    gl.shaderSource(s, src);
    gl.compileShader(s);
    gl.attachShader(prog, s);
  }
  gl.linkProgram(prog);
  gl.useProgram(prog);
  tex = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, tex);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
}

function draw() {
  requestAnimationFrame(draw);
  const dpr = window.devicePixelRatio || 1;
  const cw = Math.round(canvas.clientWidth * dpr), ch = Math.round(canvas.clientHeight * dpr);
  if (canvas.width !== cw || canvas.height !== ch) { canvas.width = cw; canvas.height = ch; seen = -1; }
  const gw = get(F.GAME_W) || 640, gh = get(F.GAME_H) || 480;
  const k = Math.min(cw / gw, ch / gh);
  box = { w: gw * k, h: gh * k };
  box.x = (cw - box.w) / 2;
  box.y = (ch - box.h) / 2;
  canvas.style.cursor = get(F.CURSOR_VISIBLE) ? 'default' : 'none';
  const seq = get(F.FRAME_SEQ);
  if (seq === seen || get(F.FRAME_FRONT) < 0) return;
  seen = seq;
  let front;
  do { front = get(F.FRAME_FRONT); set(F.FRAME_READING, front); } while (get(F.FRAME_FRONT) !== front);
  const ptr = get(F.FRAME_PTR + front) >>> 0, w = get(F.FRAME_W + front), h = get(F.FRAME_H + front);
  if (pixels.length !== w * h * 4) pixels = new Uint8Array(w * h * 4);
  pixels.set(u8.subarray(ptr, ptr + w * h * 4));  // WebGL takes no shared memory
  set(F.FRAME_READING, -1);
  const filter = get(F.SMOOTH) ? gl.LINEAR : gl.NEAREST;
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, filter);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, filter);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, pixels);
  gl.viewport(0, 0, cw, ch);
  gl.clearColor(0, 0, 0, 1);
  gl.clear(gl.COLOR_BUFFER_BIT);
  gl.viewport(Math.round(box.x), Math.round(box.y), Math.round(box.w), Math.round(box.h));
  gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}

// ---- Input

function gamePos(e) {
  const r = canvas.getBoundingClientRect(), dpr = window.devicePixelRatio || 1;
  const gw = get(F.GAME_W) || 640, gh = get(F.GAME_H) || 480;
  const x = Math.floor(((e.clientX - r.left) * dpr - box.x) / box.w * gw);
  const y = Math.floor(((e.clientY - r.top) * dpr - box.y) / box.h * gh);
  return { X: Math.max(0, Math.min(gw - 1, x)), Y: Math.max(0, Math.min(gh - 1, y)) };
}

let lastDown = 0;
function pointer(type, e) {
  const p = gamePos(e);
  set(F.MOUSE_X, p.X);
  set(F.MOUSE_Y, p.Y);
  if (type === T.MouseMove) { push(type, p); return; }
  const button = e.button === 2 ? 1 : e.button === 1 ? 2 : 0;
  let clicks = 1;
  if (type === T.MouseDown) {
    const now = performance.now();
    clicks = now - lastDown < 400 ? 2 : 1;
    lastDown = clicks === 2 ? 0 : now;
  }
  push(type, { ...p, BUTTON: button, CLICKS: clicks });
}

function key(type, e) {
  const vk = VK[e.code];
  if (!vk) return;
  e.preventDefault();
  const flags = (e.repeat ? 1 : 0) | ((IS_MAC ? e.metaKey : e.ctrlKey) ? 2 : 0);
  push(type, { VK: vk, SIDE_VK: SIDE_VK[e.code] || 0, FLAGS: flags });
  if (type === T.KeyDown && e.key.length === 1 && !e.ctrlKey && !e.metaKey) push(T.Text, {}, e.key);
}

// Full screen when the game's request changes (the options panel), on the next input:
// only during a user's input does the browser allow it. Leaving it with the browser's
// own Escape is the player's answer too.
let fullscreenAsked = 0;
function fullscreenFollow() {
  const want = get(F.FULLSCREEN_WANT) ? 1 : 0;
  if (want === fullscreenAsked) return;
  fullscreenAsked = want;
  const is = !!document.fullscreenElement;
  if (want && !is) $('game').requestFullscreen?.().catch(() => {});
  if (!want && is) document.exitFullscreen?.().catch(() => {});
}

function listen() {
  canvas.addEventListener('pointermove', (e) => pointer(T.MouseMove, e));
  canvas.addEventListener('pointerdown', (e) => { canvas.focus(); fullscreenFollow(); pointer(T.MouseDown, e); });
  canvas.addEventListener('pointerup', (e) => pointer(T.MouseUp, e));
  canvas.addEventListener('contextmenu', (e) => e.preventDefault());
  canvas.addEventListener('wheel', (e) => {
    e.preventDefault();
    push(T.Wheel, { ...gamePos(e), WHEEL_MILLI: Math.round(-Math.sign(e.deltaY) * 1000) });
  }, { passive: false });
  window.addEventListener('keydown', (e) => { fullscreenFollow(); key(T.KeyDown, e); });
  window.addEventListener('keyup', (e) => key(T.KeyUp, e));
  window.addEventListener('focus', () => push(T.FocusIn));
  window.addEventListener('blur', () => push(T.FocusOut));
  document.addEventListener('visibilitychange', () => {
    set(F.PAGE_VISIBLE, document.hidden ? 0 : 1);
    if (!document.hidden) push(T.Exposed);
  });
  document.addEventListener('fullscreenchange', () => {
    const is = document.fullscreenElement ? 1 : 0;
    set(F.FULLSCREEN_IS, is);
    set(F.FULLSCREEN_WANT, is);
    fullscreenAsked = is;
  });
}

// ---- Sound

let audio, sound;

async function startSound() {
  try {
    audio = new AudioContext({ latencyHint: 'interactive' });
    await audio.audioWorklet.addModule('sound-worklet.js');
    sound = new AudioWorkletNode(audio, 'mickey-sound', { outputChannelCount: [2] });
    sound.connect(audio.destination);
    await audio.resume();
  } catch (err) {
    console.warn('no sound:', err);
    audio = sound = null;
  }
}

async function play() {
  if (!crossOriginIsolated) {
    say('This page needs to reload once to run the game (shared memory); if it keeps showing this, the browser is not supported.');
    return;
  }
  say('Starting...');
  await startSound();  // in the click: browsers start sound only then
  await new Promise((resolve, reject) => {
    const s = document.createElement('script');
    s.src = 'Steamboat.js';
    s.onload = resolve;
    s.onerror = reject;
    document.head.appendChild(s);
  });
  $('start').hidden = true;
  $('game').style.display = 'block';
  canvas.focus();
  mod = await createMickey({
    print: (t) => console.log(t),
    printErr: (t) => console.warn(t),
    onExit(code) {
      $('game').style.display = 'none';
      $('start').hidden = false;
      say(`The game has ended (${code}).`);
      showSaves();
    },
    preRun: [(m) => {
      // before the game's main starts on its worker: the shared block, the sound's rate
      mem = m.wasmMemory;
      base = m._host_web_shared() >> 2;
      // ?log=module,vfs (or ?log=1): the port's diagnostics in the console (PORT_LOG)
      const log = new URLSearchParams(location.search).get('log');
      if (log) m.ENV.PORT_LOG = log;
      set(F.PAGE_VISIBLE, document.hidden ? 0 : 1);
      set(F.SOUND_RATE, audio ? Math.round(audio.sampleRate) : 0);
      setupGl();
      listen();
      requestAnimationFrame(draw);
    }],
  });
}

$('iso').addEventListener('change', (e) => { if (e.target.files[0]) chosen(e.target.files[0]); });
$('change').addEventListener('click', () => { $('play-row').hidden = true; $('choose-row').hidden = false; });
$('save-file').addEventListener('change', (e) => {
  if (e.target.files[0]) importSave(e.target.files[0]);
  e.target.value = '';
});
$('play').addEventListener('click', play);
showStart();
