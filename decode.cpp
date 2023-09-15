#include<iostream>
#include<cstdio>
#include<cctype>
#include<vector>
#include<cstring>
#include<algorithm>
#include<ctime>
#include "huffman.hpp"

FILE *fin,*fout,*fdic;

char rbuf[1<<15],*p1=rbuf,*p2=rbuf;

int next_char() {
    if(p1==p2) {
        p2=((p1=rbuf)+fread(rbuf,1,1<<15,fin));
        if(p1==p2) return 1024;
    }
    return *p1++;
}

char obuf[1<<15],*o1=obuf,*o2=obuf+(1<<15);

void push_char(char x) {
    *o1++=x;
    if(o1==o2) {
        fwrite(obuf,1,1<<15,fout);
        o1=obuf;
    }
}

unsigned long long length,cnt;

signed main(int argc,char **argv) {
    if(argc<3) {
        fputs("[ERROR] Too few arguments! decode.exe SOURCE DICTIONARY [TARGET]",stderr);
        exit(-1);
    }
    if(argc==3) {
        fin=fopen(argv[1],"rb");
        fdic=fopen(argv[2],"rb");
        fout=fopen(argv[1],"wb");
    }
    else if(argc==4) {
        fin=fopen(argv[1],"rb");
        fdic=fopen(argv[2],"rb");
        fout=fopen(argv[3],"wb");
    }
    else {
        fputs("[ERROR] Too many arguments! decode.exe SOURCE DICTIONARY [TARGET]",stderr);
        exit(-1);
    }
    fscanf(fdic,"%llu%llu",&length,&cnt);
    std::string str;
    char tmp_arr[50];
    DecodingTree<char> decoder;
    int nw;
    for(unsigned long long int i=0;i<cnt;i++) {
        fscanf(fdic,"%d%s",&nw,tmp_arr);
        str=tmp_arr;
        decoder.insert(nw,str);
    }
    int char_now=next_char();
    unsigned long long tot=0;
    while(char_now!=1024) {
        for(int i=7;i>=0&&tot<length;i--,tot++) {
            auto p=decoder.next_bit((char_now>>i)&1);
            if(p.first) push_char(p.second);
        }
        char_now=next_char();
    }
    fwrite(obuf,1,o1-obuf,fout);
    fclose(fin);
    fclose(fout);
    return 0;
}