"""Build exact KPK win/draw tables for files a-d (e-h are mirrored at runtime).
The historical script/asset names are kept for compatibility with older app copies.
Run: python3 tools/build-rook-pawn-table.py
"""
from collections import deque
from pathlib import Path
import base64
SIZE=6*64*64*2
xy=lambda s:(s%8,s//8)
def distance(a,b):
 x,y=xy(a);u,v=xy(b);return max(abs(x-u),abs(y-v))
neighbors=[[t for t in range(64) if distance(s,t)==1] for s in range(64)]
def index(r,a,d,t):return (((r-2)*64+a)*64+d)*2+t

def build(file):
 def pawn_attacks(p):return {p-9 if file>0 else -1,p-7}
 def promoted_win(a,d,kind):
  promotion=file
  if distance(d,promotion)==1 and distance(a,promotion)>1:return False
  def attack(target):
   x,y=xy(target);u,v=xy(promotion);dx,dy=x-u,y-v
   if not(dx==0 or dy==0 or(kind=='q' and abs(dx)==abs(dy))):return False
   sx=(dx>0)-(dx<0);sy=(dy>0)-(dy<0);cur=promotion+sx+8*sy
   while cur!=target:
    if cur==a:return False
    cur+=sx+8*sy
   return True
  legal=[t for t in neighbors[d] if t!=a and distance(t,a)>1 and(t==promotion or not attack(t))]
  return bool(legal) or attack(d)
 valid=bytearray(SIZE)
 for r in range(2,8):
  p=(r-1)*8+file;attacks=pawn_attacks(p)
  for a in range(64):
   for d in range(64):
    if a==p or d==p or distance(a,d)<=1:continue
    valid[index(r,a,d,0)]=1
    if d not in attacks:valid[index(r,a,d,1)]=1
 pred=[[] for _ in range(SIZE)];remaining=[0]*SIZE;maximum=[0]*SIZE;value=bytearray([255])*SIZE;queue=deque()
 for r in range(2,8):
  p=(r-1)*8+file;attacks=pawn_attacks(p)
  for a in range(64):
   for d in range(64):
    for turn in(0,1):
     key=index(r,a,d,turn)
     if not valid[key]:continue
     value[key]=0;edges=[];terminal_win=False
     if turn==0:
      for t in neighbors[d]:
       if t==a or distance(t,a)<=1:continue
       if t==p:edges.append(-1)  # Pawn captured: draw.
       elif t not in attacks:edges.append(index(r,a,t,1))
      if not edges and d in attacks:terminal_win=True
     else:
      for t in neighbors[a]:
       if t!=p and t!=d and distance(t,d)>1:edges.append(index(r,t,d,0))
      if p-8 not in(a,d):
       if r==2:
        if promoted_win(a,d,'q') or promoted_win(a,d,'r'):terminal_win=True
        else:edges.append(-1)
       else:
        edges.append(index(r-1,a,d,0))
        if r==7 and p-16 not in(a,d):edges.append(index(r-2,a,d,0))
     remaining[key]=len(edges)
     for t in edges:
      if t>=0:
       assert valid[t],(file,r,a,d,turn,t)
       pred[t].append(key)
     if terminal_win:value[key]=1;queue.append(key)
 while queue:
  child=queue.popleft()
  for parent in pred[child]:
   if value[parent]:continue
   if parent%2:value[parent]=value[child]+1;queue.append(parent)
   else:
    remaining[parent]-=1;maximum[parent]=max(maximum[parent],value[child])
    if remaining[parent]==0:value[parent]=maximum[parent]+1;queue.append(parent)
 print('File','abcd'[file],':',sum(valid),'legal states, maximum winning distance',max(x for x in value if x!=255))
 return value
combined=b''.join(build(file) for file in range(4))
encoded=base64.b64encode(combined).decode();chunks=[encoded[i:i+120] for i in range(0,len(encoded),120)]
source='// Generated exact king+pawn vs king table: files a-d, files e-h mirrored.\n// 0 draw; 1..254 forced-win distance; 255 illegal. Rebuild with tools/build-rook-pawn-table.py.\nwindow.PAWN_ENDGAME_TABLE=Uint8Array.from(atob([\n'+',\n'.join('"'+s+'"' for s in chunks)+'].join("")),c=>c.charCodeAt(0));\nwindow.ROOK_PAWN_TABLE=window.PAWN_ENDGAME_TABLE.subarray(0,49152);\n'
(Path(__file__).resolve().parents[1]/'dist/rook-pawn-table.js').write_text(source)
