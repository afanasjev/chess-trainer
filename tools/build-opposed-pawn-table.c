// Export a symmetric KPKP WDL+DTZ table and exact promotion boundary results.
#define TB_NO_THREADS
#include "tbprobe.c"
#include <stdio.h>
#include <stdlib.h>
static uint64_t bit(int s){return UINT64_C(1)<<s;}
static unsigned char export_value(Pos *pos,int withDistance){if(!is_valid(pos))return 255;int ok=1,w=probe_wdl(pos,&ok);if(!ok){fprintf(stderr,"WDL probe failed\n");exit(2);}if(w==0||w==1||w==-1)return 0;int winner=(w>0)==pos->turn?1:2;if(!withDistance)return winner;int z=probe_dtz(pos,&ok);if(!ok){fprintf(stderr,"DTZ probe failed\n");exit(3);}int d=abs(z);if(d<1)d=1;if(d>126)d=126;return (winner==1?0:128)+d;}
int main(int argc,char **argv){if(argc!=4)return 1;if(!tb_init(argv[1]))return 2;FILE *f=fopen(argv[2],"wb");for(int r=1;r<=6;r++){for(int file=0;file<4;file++){int p=r*8+file;for(int q=8;q<56;q++)for(int a=0;a<64;a++)for(int d=0;d<64;d++)for(int t=0;t<2;t++){Pos pos={bit(a)|bit(p),bit(d)|bit(q),bit(a)|bit(d),0,0,0,0,bit(p)|bit(q),0,0,t};unsigned char v=export_value(&pos,1);fwrite(&v,1,1,f);}}fprintf(stderr,"KPKP rank %d exported\n",r+1);}fclose(f);f=fopen(argv[3],"wb");for(int kind=0;kind<4;kind++){for(int file=0;file<4;file++){int p=56+file;for(int q=8;q<56;q++)for(int a=0;a<64;a++)for(int d=0;d<64;d++){Pos pos={bit(a)|bit(p),bit(d)|bit(q),bit(a)|bit(d),kind==0?bit(p):0,kind==1?bit(p):0,kind==2?bit(p):0,kind==3?bit(p):0,bit(q),0,0,false};unsigned char v=export_value(&pos,0);fwrite(&v,1,1,f);}}fprintf(stderr,"Promotion kind %d exported\n",kind);}fclose(f);tb_free();return 0;}
