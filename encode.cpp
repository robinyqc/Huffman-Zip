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

unsigned long long buf[256],*cnt=buf+128;

std::string sbuf[256],*code=sbuf+128;

signed main(int argc,char **argv) {
    std::string dic_dir;
    if(argc==1) {
        fputs("[ERROR] Too many arguments! decode.exe SOURCE [DICTIONARY] [TARGET]",stderr);
        exit(-1);
    }
    else if(argc==2) {
        fin=fopen(argv[1],"rb");
        dic_dir=argv[1];
        std::string tmp(argv[1]);
        tmp+=".target_zip_file";
        fout=fopen(tmp.c_str(),"wb");
    }
    else if(argc==3) {
        if(strcmp(argv[1],argv[2])==0) {
            fputs("[ERROR] Source file and target file have same name.",stderr);
            exit(-1);
        }
        fin=fopen(argv[1],"rb");
        dic_dir=argv[1];
        fout=fopen(argv[2],"wb");
    }
    else {
        fputs("[ERROR] Too many arguments! decode.exe SOURCE [DICTIONARY] [TARGET]",stderr);
        exit(-1);
    }
    dic_dir+=".dictionary";
    fdic=fopen(dic_dir.c_str(),"wb");
    int char_now=next_char();
    while(char_now!=1024) {
        cnt[char_now]++;
        char_now=next_char();
    }
    HuffmanTree<char> transcoder;
    std::vector<std::pair<char,unsigned long long> > count_info;
    for(int i=-128;i<128;i++) if(cnt[i]>0) count_info.emplace_back(i,cnt[i]);
    auto new_code=transcoder.transcode(count_info);
    for(auto &p:new_code) code[(int)p.first]=p.second;
    fseek(fin,0,SEEK_SET);
    unsigned long long tot=0;
    char_now=next_char();
    char tr=0,x=0;
    while(char_now!=1024) {
        for(char i:code[char_now]) {
            if(i=='1') tr=tr<<1|1;
            else tr=tr<<1;
            x++,tot++;
            if(x==8) push_char(tr),x=tr=0;
        }
        char_now=next_char();
    }
    fclose(fin);
    while(x<8) tr<<=1,x++;
    push_char(tr);
    fwrite(obuf,1,o1-obuf,fout);
    fclose(fout);
    fprintf(fdic,"%llu %d\n",tot,(int)new_code.size());
    for(auto &p:new_code) fprintf(fdic,"%d %s\n",(int)p.first,p.second.c_str());
    fputs("Successfully zipped.",stderr);
    fclose(fdic);
    return 0;
}