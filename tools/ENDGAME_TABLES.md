# Offline educational endgame tables

The browser loads only the generated JavaScript data. No engine, server or network
connection is needed during a training session.

## One pawn / two pawns of the same colour (chapters 1–2)

`build-rook-pawn-table.py` builds the KPK forced-win attractor.
`build-two-pawn-table.py` builds KPPK, using KPK captures and safe Q/R promotions
as boundaries. These tables use a distance to a winning conversion.

## One pawn per side (chapter 3)

`build-opposed-pawn-table.py --tables /path/to/syzygy` builds:

- `dist/opposed-pawn-table.js`: KPKP WDL and DTZ, mirrored so that the white
  pawn is on files a–d. Values 0 mean draw; 1–126 white wins; 129–254 black wins;
  low seven bits give DTZ; 255 means an illegal state.
- A promotion-boundary table in the same asset, for Q/R/B/N versus a pawn.
  It records the outcome immediately after promotion, including capture,
  stalemate, counter-promotion and drawing resources. It is not an assumption
  that whoever promotes first wins.

Required Syzygy files:

- WDL `.rtbw`: KPvKP, KQvKP, KRvKP, KBvKP, KNvKP, KPvK, KQvK, KRvK, KBvK, KNvK.
- DTZ `.rtbz`: KPvKP, KPvK.

The files used for the initial export came from
<https://tablebase.lichess.ovh/tables/standard/3-4-5-wdl/> and
<https://tablebase.lichess.ovh/tables/standard/3-4-5-dtz/>.
Their SHA-256 hashes are in `opposed-pawn-table-sources.json`. The builder performs
no downloads. Original tablebase files are not part of this repository.

The exporter uses the vendored MIT-licensed Fathom sources from
<https://github.com/jdart1/Fathom> (retrieved 2026-10-26). The original licence is
in `vendor/fathom/LICENSE`. Syzygy was created by Ronald de Man; Fathom's README
describes the provenance and unrestricted use of its probing code.

The browser handles en-passant as an additional legal capture when comparing
outcomes. Captures to KPK use the chapter-one table. Fifty-move cursed wins and
blessed losses are treated as draws. Actual repetition and fifty-move draws are
checked against the played game's history when choosing moves.

Training stops at a game result or at promotion. At promotion the result is
explicitly labelled as the tablebase assessment of the resulting ending; the
trainer does not declare the game over merely because a queen appeared. The
post-promotion queen/rook/minor-piece ending is outside this pawn lesson.

Run `node tools/test-opposed-pawn-oracle.cjs` to check legal successors,
mirroring/colour swaps, all book diagrams and sample lines, en-passant and a drawn
queen-versus-pawn conversion. `node tools/test-chapter-three.cjs` exercises every
new task for both playing sides, completion, restart and stored progress.
