//
// Created by 김세희 on 2017. 12. 8..
//

#include "hmm.h"
#include "viterbi.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <zconf.h>
#include <cmath>

using namespace std;

map <pair<string,string>, double > bigram;
map <string,vector<string> > dictionary;
map <string,double> unigram;
//map <string,vector<vector<double>,


int indent=0;
void get_data();
void datafromtst(string path)
void tstinput();
void make_hmm();
int main(){
    get_data();
    tstinput();
    make_hmm();
    //viterbi();
    return 0;
}

void get_data(){
    ifstream fb;
    fb.open("bigram.txt",ios::in);
    string tmpstr,buf,a1,a2,v;

    while(!fb.eof()){
        getline(fb,tmpstr);
        stringstream ss(tmpstr);
        ss>>a1;ss>>a2;ss>>v;

        //cout << stod(v) << endl;
        bigram.insert(make_pair(make_pair(a1,a2),stod(v)));

        //bigram.insert(make_pair(make_pair))
    }
    fb.close();

    fb.open("dictionary.txt",ios::in);
    getline(fb,tmpstr); //<s> sil
    vector<string> seq;

    while(!fb.eof()){
        getline(fb,tmpstr);
        stringstream ss(tmpstr);
        ss>>a1;
        while(ss>>a2){
            seq.push_back(a2);
        }
        dictionary.insert(make_pair(a1,seq));
    }
    fb.close();

    fb.open("./unigram",ios::in);
    while(!fb.eof()){
        getline(fb,tmpstr);
        stringstream ss(tmpstr);
        ss>>a1;ss>>v;
        unigram.insert(make_pair(a1,stod(v)));
    }
    fb.close();
}

void tstinput(){
    ifstream fb;
    fb.open("./filename.txt",ios::in);
    string filepath;

    while(!fb.eof()){
        getline(fb,filepath);
        datafromtst(filepath);
    }

}
void datafromtst(string path) {
    ifstream fb;
    double t;
    int n;
    string buf,a;

    vector<vector<double>> res;

    fb.open(path,ios::in);
    getline(fb,buf);
    stringstream ss(buf);
    ss>>a;ss>>a;n=stoi(a);

    while(!fb.eof()){
        vector<double> l;
        getline(fb,buf);
        stringstream ss(buf);
        for(int i=0;i<n;i++){
            ss>>a;
            l.push_back(stod(a));
        }
        res.push_back(l);
    }

    int state_size=sizeof(phones)/sizeof(hmmType);
    int tst_size=res.size();
    vector<double> pdelta(state_size);
    vector<double> delta(state_size);
    //제일 가까운 그래프가 구하기 log

    return;
}

void make_hmm(){


}
