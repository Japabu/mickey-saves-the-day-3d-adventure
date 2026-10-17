// copy-worker.js - copies the chosen ISO into the site's private storage (OPFS) as
// cd.iso, where the game reads it (writing a file there needs a worker), then writes
// cd.ok with its name and size.
onmessage = async (e) => {
  const file = e.data.file;
  try {
    const root = await navigator.storage.getDirectory();
    try { await root.removeEntry('cd.ok'); } catch (_) {}
    const iso = await (await root.getFileHandle('cd.iso', { create: true })).createSyncAccessHandle();
    iso.truncate(0);
    const CHUNK = 8 << 20;
    for (let at = 0; at < file.size; at += CHUNK) {
      const bytes = new Uint8Array(await file.slice(at, at + CHUNK).arrayBuffer());
      iso.write(bytes, { at });
      postMessage({ progress: (at + bytes.length) / file.size });
    }
    iso.flush();
    iso.close();
    const ok = await (await root.getFileHandle('cd.ok', { create: true })).createSyncAccessHandle();
    ok.truncate(0);
    ok.write(new TextEncoder().encode(JSON.stringify({ name: file.name, size: file.size })), { at: 0 });
    ok.close();
    postMessage({ done: true });
  } catch (err) {
    postMessage({ error: String(err) });
  }
};
