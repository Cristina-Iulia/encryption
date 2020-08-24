#include <stdio.h>
#include<x86intrin.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<time.h>

#define LCS _lrotl  //left circular shift
#define RCS _lrotr  //right circular shift
#define u64 unsigned long long
#define R(x,y,k) (x = RCS(x,8) , x += y , x ^= k , y = LCS(y,3) , y ^= x)
#define INV_R(x,y,k) (y ^= x , y =RCS(y,3) , x ^= k , x -= y , x = LCS(x,8) )

void Speck128ExpandKeyAndEncrypt(u64 pt[] , u64 ct[] , u64 K[]){

    u64 i , B = K[1] , A = K[0];
    ct[0] = pt[0];
    ct[1] = pt[1];
    for(i = 0 ; i< 32 ; ++i){
        R(ct[1] , ct[0] , A);
        R(B , A , i);
    }
}
void Speck128Encrypt(u64 pt[] , u64 ct[] , u64 k[]){

    u64 i;
    ct[0] = pt[0];
    ct[1] = pt[1];
    for(i = 0 ; i < 32 ; ++i)
        R(ct[1], ct[0] , k[i]);
}
void Speck128Decrypt(u64 pt[] , u64 ct[] , u64 K[]){

    long long i;
    ct[0]=pt[0];
    ct[1]=pt[1];
    for(i = 31 ; i >= 0; --i){
        INV_R(ct[1] , ct[0] , K[i]);
    }
}
static void speck_keyexpand(u64 K[]){

    u64 tmp[32] , *p;
    memcpy(tmp , K , sizeof(tmp));

    u64 i;
    for(i = 0 ; i < (31);){
        p = tmp + (1 + (i % (2 - 1)));
        R(*p , tmp[0] , i);
        ++i;
        K[i] = tmp[0];
    }
}

u64 get_key(){

      char s[32];
      printf("\nPassword: ");
      scanf("%32s",s );
      u64 nr=0x0;
      for(int i=1; i<=8; i++){
      nr=nr|s[0];
      nr=nr<<8;
    }
    return nr;
}
void expand_file(char *s){

  FILE *fp=fopen(s,"a");
  if(!fp){
    printf("\nError at opening file");
    exit(EXIT_FAILURE);
  }
  fseek(fp,0,SEEK_END);
  u64 size=ftell(fp);
  fseek(fp,0,SEEK_SET);

  if(size%16 == 0)return;
  for(int i=1; i<=16-size%16; i++){
    fwrite("0",sizeof(char),1,fp);


}
fclose(fp);
}

int main(int argc, char **argv){
  if(argc<3){
    printf("\nInvalid number of arguments");
    return 0;
  }

  FILE *fp=fopen(argv[2],"rb");
  if(!fp){
    printf("\nError at opening file");
    return 0;
  }

expand_file(argv[2]);

  FILE *f=fopen(argv[1],"wb");
  if(!f){
    printf("\nError at opening file");
    return 0;
  }


  u64 k[32];
  k[1] = get_key();
  k[0] = k[1] + 25711;

  u64 pt[2];
  speck_keyexpand(k);

  while(fread(&pt,sizeof(pt),1,fp)){

  u64 ct[] = {0x0,0x0};


  if(strcmp(argv[1],"encrypt") == 0){
  //Speck128ExpandKeyAndEncrypt(pt, ct, k)

  Speck128Encrypt(pt, ct, k);
  fwrite(&ct,sizeof(ct),1,f);
}

  if(strcmp(argv[1],"decrypt") == 0){
    Speck128Decrypt(pt,ct,k);
    fwrite(&ct,sizeof(ct),1,f);


  }

}

fclose(fp);
fclose(f);
return 0;
}
