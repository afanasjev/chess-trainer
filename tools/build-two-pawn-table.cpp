// Exact finite retrograde attractor for KPPK, with same-colour pawns.
// Normalized to white pawns; t=1 means the pawn side moves.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <vector>
using namespace std;
int pairs[64][64],P[1128],Q[1128],countP=0;vector<int> nb[64];vector<uint8_t> val,deg,mx,kpk;vector<uint32_t> pending;
int dist(int a,int b){return max(abs(a%8-b%8),abs(a/8-b/8));}
bool attack(int p,int s){return s/8==p/8+1&&abs(s%8-p%8)==1;}
int key(int p,int q,int a,int d,int t){if(p>q)swap(p,q);return ((pairs[p][q]*64+a)*64+d)*2+t;}
bool legal(int p,int q,int a,int d,int t){return a!=p&&a!=q&&d!=p&&d!=q&&dist(a,d)>1&&(!t||(!attack(p,d)&&!attack(q,d)));}
int one(int p,int a,int d,int t){p^=56;a^=56;d^=56;if(p%8>3){p^=7;a^=7;d^=7;}return kpk[(p%8)*49152+(((p/8-1)*64+a)*64+d)*2+t];}
bool promoted(int p,int q,int a,int d){ // white promotes p to queen or rook; black to move
 if(dist(d,p)==1&&dist(a,p)>1&&!attack(q,p)){int v=one(q,a,p,1);if(v==0)return false;}
 for(int kind=0;kind<2;kind++){
  auto ray=[&](int s){int dx=s%8-p%8,dy=s/8-p/8;if(!(dx==0||dy==0||(!kind&&abs(dx)==abs(dy))))return false;int sx=(dx>0)-(dx<0),sy=(dy>0)-(dy<0);for(int cur=p+sx+8*sy;cur!=s;cur+=sx+8*sy)if(cur==a||cur==q)return false;return true;};
  bool move=false;for(int s:nb[d]){if(s==a||dist(s,a)<=1)continue;if(s==p){if(!attack(q,s))move=true;}else if(!ray(s)&&(s==q||!attack(q,s)))move=true;}
  if(move||ray(d)||attack(q,d))return true;
 }return false;
}
void win(int k,int distance){if(!val[k]){val[k]=min(254,distance);pending.push_back(k);}}
int main(int argc,char**argv){if(argc!=3){fprintf(stderr,"Usage: build kpk.bin kpp.bin\n");return 1;}ifstream in(argv[1],ios::binary);kpk.assign(istreambuf_iterator<char>(in),{});for(int p=8;p<56;p++)for(int q=p+1;q<56;q++){pairs[p][q]=countP;P[countP]=p;Q[countP++]=q;}for(int a=0;a<64;a++)for(int b=0;b<64;b++)if(dist(a,b)==1)nb[a].push_back(b);int N=countP*8192;val.assign(N,255);deg.resize(N);mx.resize(N);pending.reserve(N);
 for(int z=0;z<countP;z++){int p=P[z],q=Q[z];for(int a=0;a<64;a++)for(int d=0;d<64;d++)for(int t=0;t<2;t++)if(legal(p,q,a,d,t)){int k=key(p,q,a,d,t);val[k]=0;int n=0;bool seed=false;
 if(t){for(int s:nb[a])if(s!=p&&s!=q&&dist(s,d)>1)n++;for(int x:{p,q}){int other=x==p?q:p;if(x+8!=a&&x+8!=d&&x+8!=other){if(x/8==6){n++;if(promoted(x+8,other,a,d))seed=true;}else{n++;if(x/8==1&&x+16!=a&&x+16!=d&&x+16!=other)n++;}}}}
 else{for(int s:nb[d]){if(dist(s,a)<=1)continue;if(s==p||s==q){if(attack(s==p?q:p,s))continue;n++;int v=one(s==p?q:p,a,s,1);if(v>0&&v<255){mx[k]=max<int>(mx[k],v);n--;}}else if(!attack(p,s)&&!attack(q,s))n++;}if(n==0&&(mx[k]||attack(p,d)||attack(q,d)))seed=true;}
 deg[k]=n;if(seed)win(k,t?1:mx[k]+1);
 }}fprintf(stderr,"seed %zu states %d\n",pending.size(),N);
 for(size_t qi=0;qi<pending.size();qi++){int k=pending[qi],t=k%2,u=k/2,d=u%64;u/=64;int a=u%64,z=u/64,p=P[z],q=Q[z];
 auto parent=[&](int pp,int qq,int aa,int dd,int tt){if(!legal(pp,qq,aa,dd,tt))return;int par=key(pp,qq,aa,dd,tt);if(val[par])return;if(tt)win(par,val[k]+1);else{mx[par]=max(mx[par],val[k]);if(--deg[par]==0)win(par,mx[par]+1);}};
 if(t){for(int s:nb[d])if(s!=p&&s!=q)parent(p,q,a,s,0);}
 else{for(int s:nb[a])if(s!=p&&s!=q)parent(p,q,s,d,1);for(int x:{p,q}){int other=x==p?q:p;if(x>=16&&x-8!=other&&x-8!=a&&x-8!=d){parent(x-8,other,a,d,1);if(x/8==3&&x-16!=other&&x-16!=a&&x-16!=d)parent(x-16,other,a,d,1);}}}
 }ofstream out(argv[2],ios::binary);out.write((char*)val.data(),val.size());fprintf(stderr,"won %zu states\n",pending.size());}
