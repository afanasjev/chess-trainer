# Lessons 29–64 and chapter 10

Source: Pavel Lobach, «Пешечный эндшпиль. 64 урока», 2023,
user-provided PDF. Printed pages are PDF pages minus one.
The PDF itself is not distributed with the app.

The app includes 308 diagrams in lessons 29–64 (chapters 4–9), and
108 diagrams numbered 101–208 in chapter 10. The practicum is split into
six finite blocks of 18 positions; internal ids 101–106 are not lesson numbers.
Earlier lesson ids and saved progress keys are preserved.

Each new position links to its diagram and solution PDF page. For examples
explained in the theory rather than the answer appendix, the answer link
points to that theory page. Summaries are short original paraphrases.

## Scope of practice

“По книге” is a teaching-line exercise. Legal moves outside the selected
lines are explicitly described as outside the exercise, not as chess errors.
Completing a line marks a *control point*, not a declaration that the game
has ended. Selected branches are legal and preserve every available verified
outcome. Bad illustrative branches and unresolved parser fragments are omitted;
this is not a claim to include every variation in the printed analysis.

“Свободная практика” follows the same lines while possible, then uses the
bundled Stockfish 19 Lite worker offline. It can end at a book control point,
a real game result, or by stopping the attempt. Stopping is recorded as
unfinished and does not complete the task or invent a draw. Worker results
are invalidated on reset, catalogue changes, tab changes and page hiding.
See `dist/vendor/stockfish/SOURCE.md` for provenance and GPLv3 licensing.

## Assessment and source corrections

305 new starting positions were checked against Syzygy (up to seven pieces)
using Fathom locally or the Lichess tablebase API during preparation.
110 others use the book's assessment. Position 202 uses a preliminary engine
assessment, visibly labelled as such; it is not presented as an exact result.
Fifty-move cursed wins and blessed losses are treated as draws.

Tables are preparation-time verification, not a general multi-pawn oracle
shipped with these chapters. The older chapters retain their exact local tables.

Explicit corrections are displayed in the app:

- 34.2, 53.1, 54.1, 54.2, 102 and 103: starting turn follows the solution
  where the caption is absent or inconsistent.
- 35.8: printed diagram is drawn, but the answer's 1.d3 loses. A checked
  drawing continuation beginning 1.Ke1 is supplied instead.
- 43.8: the printed diagram wins after 1.Kxh5; the answer's 1.Kf5 loses the
  win. A tablebase continuation is supplied instead.
- 165: explicitly labelled teaching reconstruction, adding a white pawn on
  c3. The printed board lacks a c-pawn, making the stated solution impossible.
  The original FEN is retained in `sourceFen`; c3 is a reconstruction, not a
  claim about a verified original correction.
- 197: white pawn g2 restored from the identical position 169 and its solution.
  The original diagram and the change are retained and labelled.

Very short book control points in 29.7, 40.4 and 40.8 are intentional.

## Checks

`node tools/test-remaining-chapters.cjs` checks all selected lines for legal
moves, plays all 416 tasks for both colours, tests finite boundaries, restart,
persistence, practicum navigation, legal off-line moves, stopping without a
false result, and rejection of stale worker responses.

`node tools/test-endgame-engine.cjs` starts the actual embedded WASM in an
isolated browser-like worker with a file origin and disabled network.
The earlier chapter and oracle tests cover regression behaviour.
