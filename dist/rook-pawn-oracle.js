(()=>{'use strict';
const toIndex=s=>'abcdefgh'.indexOf(s[0])+(Number(s[1])-1)*8;
function evaluate(game){const pieces=game.board().flat().filter(Boolean),pawn=pieces.find(p=>p.type==='p'),promoted=pieces.find(p=>!['k','p'].includes(p.type));if(!pawn){if(!promoted)return {win:false,distance:0};const defender=promoted.color==='w'?'b':'w';if(game.isStalemate()||game.isInsufficientMaterial())return {win:false,distance:0};if(game.turn()===defender&&game.moves({verbose:true}).some(m=>m.captured&&m.to===promoted.square))return {win:false,distance:0};return {win:true,distance:0}}
if(pieces.length!==3)return null;
const attacker=pieces.find(p=>p.type==='k'&&p.color===pawn.color),defender=pieces.find(p=>p.type==='k'&&p.color!==pawn.color);let ps=toIndex(pawn.square),a=toIndex(attacker.square),d=toIndex(defender.square);if(pawn.color==='w'){ps^=56;a^=56;d^=56}if(ps%8>3){ps^=7;a^=7;d^=7}const rank=Math.floor(ps/8)+1,turn=game.turn()===pawn.color?1:0,file=ps%8,key=file*49152+(((rank-2)*64+a)*64+d)*2+turn,value=window.PAWN_ENDGAME_TABLE[key];if(value===255||value===undefined)throw Error('Invalid KPK state: '+game.fen());return {win:value>0,distance:value,attacker:a,defender:d,pawnRank:rank,pawnFile:file,pawnColor:pawn.color}}
window.PawnEndgameOracle={evaluate};window.RookPawnOracle=window.PawnEndgameOracle;
})();
