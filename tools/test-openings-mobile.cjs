const fs=require('fs'),vm=require('vm'),path=require('path'),assert=require('assert');
const html=fs.readFileSync(path.join(__dirname,'../standalone/openings.html'),'utf8');
assert(!/<script[^>]+src=/i.test(html));assert(!/<link[^>]+(?:stylesheet|preload)/i.test(html));assert(html.includes('viewport-fit=cover'));assert(!html.includes('endgameBoard'));assert(!html.includes('id="coordsTab"'));
const ids=[...html.matchAll(/\bid="([^"]+)"/g)].map(x=>x[1]);assert.equal(new Set(ids).size,ids.length);
const make=()=>({value:'',children:[],classList:{toggle(){}},setAttribute(k,v){this[k]=v},append(x){this.children.push(x)},replaceChildren(f){this.children=f.children},focus(){},select(){}});
const nodes=Object.fromEntries(ids.map(id=>[id,make()])),events={},timers=new Map(),storage={};let counter=0,now=0;
nodes.openingSide.value='b';nodes.openingRandomScope.value='chapter';
const document={hidden:false,getElementById:id=>{assert(nodes[id],'Missing HTML element: '+id);return nodes[id]},createElement:make,createDocumentFragment:make,addEventListener:(type,f)=>events[type]=f};
const ctx={document,console,Intl,Date,Math,performance:{now:()=>now+=123},localStorage:{getItem:k=>storage[k]||null,setItem:(k,v)=>storage[k]=v},setTimeout:f=>{timers.set(++counter,f);return counter},clearTimeout:id=>timers.delete(id)};ctx.window=ctx;vm.createContext(ctx);
const scripts=[...html.matchAll(/<script>([\s\S]*?)<\/script>/g)].map(x=>x[1]);let app=scripts.pop();const end=app.lastIndexOf('\n})();');app=app.slice(0,end)+'\nwindow.test={state:()=>({study,game,running,ply,line,studying,bothSides,lineIndices,lineCursor,history,waitingForNext})};'+app.slice(end);for(const script of [...scripts,app])vm.runInContext(script,ctx);
const state=ctx.test.state,lib=ctx.OPENING_LIBRARY;
assert.equal(lib.length,6);assert.equal(new Set(lib.map(c=>c.studyId)).size,3);assert.equal(lib.reduce((n,c)=>n+c.lines.length,0),28);
for(const chapter of lib){assert(chapter.pgn.startsWith('data:application/x-chess-pgn;'));const pgn=decodeURIComponent(chapter.pgn.split(',').slice(1).join(','));assert(pgn.includes(chapter.url));}
function flush(){let limit=300;while(timers.size){assert(limit-->0);const [id,f]=timers.entries().next().value;timers.delete(id);f()}}
function click(square){const b=nodes.openingBoard.children.find(b=>b['aria-label'].split(' ')[0]===square);assert(b);b.onclick()}
function select(chapter,index){nodes.openingStudy.value=chapter.studyId;nodes.openingStudy.onchange();nodes.openingChapter.value=chapter.id;nodes.openingChapter.onchange();nodes.openingLine.value=String(index);nodes.openingLine.onchange()}
function complete(){flush();let limit=150;while(state().running){assert(limit-->0);const m=state().line[state().ply];click(m.from);click(m.to);flush()}}
let runs=0;for(const chapter of lib)for(const role of ['w','b','both'])for(let i=0;i<chapter.lines.length;i++){select(chapter,i);if(state().bothSides!==(role==='both'))nodes.openingBoth.onclick();nodes.openingSide.value=role==='both'?'w':role;nodes.openingSide.onchange();nodes.openingStart.onclick();complete();assert.equal(nodes.openingStatus.textContent,'Вариант пройден');assert.equal(state().game.history().length,chapter.lines[i].length);assert.equal(state().history.at(-1).errors,0);runs++}
assert.equal(runs,84);
// Repeat restarts the current line, and study mode can step both ways without creating results.
nodes.openingRepeat.onclick();assert.equal(state().ply,0);const before=state().history.length;nodes.openingStudyMode.onclick();assert(state().studying);nodes.openingForward.onclick();assert.equal(state().ply,1);nodes.openingBack.onclick();assert.equal(state().ply,0);assert.equal(state().history.length,before);nodes.openingStudyMode.onclick();
// Whole-study random queue includes each line of each chapter once and stops at the end.
select(lib.find(c=>c.studyId==='cbgAvVjL'),0);nodes.openingRandomScope.value='study';nodes.openingRandom.onclick();nodes.openingStart.onclick();assert.equal(state().lineIndices.length,7);assert.equal(new Set(state().lineIndices.map(t=>t.chapterId+':'+t.lineIndex)).size,7);const visited=new Set();do{visited.add(state().study.id);complete();if(!state().waitingForNext)break;nodes.openingNext.onclick();assert.equal(state().ply,0)}while(true);assert.equal(visited.size,2);assert.equal(nodes.openingStatus.textContent,'Все варианты пройдены');assert.equal(nodes.openingNext.hidden,true);
// Going into the background cancels the pending reply and resumes on return.
nodes.openingRandom.onclick();if(state().bothSides)nodes.openingBoth.onclick();nodes.openingSide.value='b';nodes.openingStart.onclick();assert(timers.size);document.hidden=true;events.visibilitychange();assert.equal(timers.size,0);assert.equal(state().ply,0);document.hidden=false;events.visibilitychange();flush();assert.equal(state().ply,1);
assert.equal(nodes.openingBoard.children.length,64);assert(storage['chess-openings-v1']);
console.log('PASS: embedded app, all 6 chapters / 28 lines / 84 playthroughs, study, repeat, random queue across chapters, background/resume, PGN exports, no external scripts or phantom tabs.');
