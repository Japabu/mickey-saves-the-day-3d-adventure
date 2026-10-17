// crash.pre.js - put before the game's JavaScript (--pre-js): a trap in the game's worker
// (a RuntimeError such as "memory access out of bounds") reaches the page only as its
// message; this prints its stack there too, through err(), which the worker forwards to
// the page's printErr. With --profiling-funcs the stack names the game's functions.
if (typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD) {
  self.addEventListener('error', (e) => {
    var stack = e.error && e.error.stack;
    if (stack)
      err('[crash] ' + stack);
  });
}
