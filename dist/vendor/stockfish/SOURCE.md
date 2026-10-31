# Stockfish.js 19.0.1 Lite (single-threaded)

Copyright Stockfish developers and Chess.com. Licensed under GPLv3; see COPYING.

Upstream source and build instructions: https://github.com/nmrugg/stockfish.js/tree/v19.0.1
Original release: https://github.com/nmrugg/stockfish.js/releases/tag/v19.0.1
Package: https://registry.npmjs.org/stockfish/-/stockfish-19.0.1.tgz

The worker bundles `stockfish-19-lite-single.js` and its matching `.wasm`.
The loader supplies embedded WASM bytes through Emscripten's `wasmBinary`
option and `instantiateWasm` hook, replacing the download-progress fetch wrapper. There are no engine changes. This avoids fetching a WASM file from file://.
The JavaScript source and WASM are retained within `worker-data.js`; the latter
can be recovered by decoding its `__TRAINER_WASM` base64 literal.
The worker runs only in the optional free-practice mode, and is terminated on
reset or leaving the endgame view. It is not used to replace the book's teaching
lines. Its evaluation is labelled as engine analysis, never an exact tablebase.

Original JS SHA-256: ccb130d7ff29408531c47d859a49f91d59d4f482d05a53eb1a3231a4f624c91a
Original WASM SHA-256: ee4f115bdb2d32cadf96cada104a05b5ec257d7ad6f57582190b49bc71f096de
