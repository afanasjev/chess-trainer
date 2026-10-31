"""Rebuild the offline KPPK table. Requires a C++17 compiler, no Python packages.
The solved boundary is a safe Q/R promotion or a won KPK after a capture.
Draws are states outside the forced-win attractor. No network is used.
"""
from pathlib import Path
import base64, re, subprocess, tempfile, os
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as temp:
 temp=Path(temp)
 encoded=''.join(re.findall(r'"([A-Za-z0-9+/=]+)"',(root/'dist/rook-pawn-table.js').read_text()))
 (temp/'kpk.bin').write_bytes(base64.b64decode(encoded))
 subprocess.run([os.environ.get('CXX','c++'),'-O3','-std=c++17',str(root/'tools/build-two-pawn-table.cpp'),'-o',str(temp/'build')],check=True)
 subprocess.run([str(temp/'build'),str(temp/'kpk.bin'),str(temp/'kpp.bin')],check=True)
 b=(temp/'kpp.bin').read_bytes()
o=bytearray();seen={};i=0;anchor=0

def ext(n):
 while n>=255:o.append(255);n-=255
 o.append(n)
while i+4<=len(b):
 key=b[i:i+4];prev=seen.get(key,-100000);seen[key]=i
 if i-prev>65535 or b[prev:prev+4]!=key:i+=1;continue
 length=4
 while i+length<len(b) and b[prev+length]==b[i+length]:length+=1
 lit=i-anchor;o.append((min(lit,15)<<4)|min(length-4,15))
 if lit>=15:ext(lit-15)
 o.extend(b[anchor:i]);offset=i-prev;o.extend((offset&255,offset>>8))
 if length-4>=15:ext(length-19)
 end=i+length
 for j in range(i+1,end):
  if j+4<=len(b):seen[b[j:j+4]]=j
 i=end;anchor=i
lit=len(b)-anchor;o.append(min(lit,15)<<4)
if lit>=15:ext(lit-15)
o.extend(b[anchor:]);s=base64.b64encode(o).decode();chunks=[s[i:i+120] for i in range(0,len(s),120)]
source='// Exact KPPK attractor table. LZ4 block; rebuild with tools/build-two-pawn-table.py.\n(()=>{const b=Uint8Array.from(atob([\n'+',\n'.join('"'+c+'"' for c in chunks)+'].join("")),c=>c.charCodeAt(0)),out=new Uint8Array(9240576);let i=0,j=0;const length=n=>{if(n===15){let x;do{x=b[i++];n+=x}while(x===255)}return n};while(i<b.length){const tag=b[i++],lit=length(tag>>4);out.set(b.subarray(i,i+lit),j);i+=lit;j+=lit;if(i===b.length)break;const offset=b[i++]|(b[i++]<<8),n=length(tag&15)+4;for(let k=0;k<n;k++){out[j]=out[j-offset];j++}}if(j!==out.length)throw Error("Invalid KPPK table");window.TWO_PAWN_TABLE=out})();\n'
(root/'dist/two-pawn-table.js').write_text(source);print('compressed',len(o),'source',len(source))
