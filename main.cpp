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
#define _USE_MATH_DEFINES
double min_double = -9999999999;
int state_size ;//= sizeof(phones) / sizeof(hmmType);
int syl_N=sizeof(phones)/sizeof(hmmType);

map<pair<string, string>, double> bigram;
map<string, vector<string> > dictionary;
map<string, double> unigram;
map<string, hmmType2> phone2;
vector<stateType> states;
//map <string,vector<vector<double>,
vector<double> pdfsum[1000];
double hmm_tp[200][200];

int indent = 0;

void get_data();

vector<string> datafromtst(string path);

void tstinput();

double stateprob(vector<double>, int);

void make_hmm();
void vec_phone();
void prev_pdfsum();

int main() {
    get_data();
    make_hmm();
    prev_pdfsum();
    tstinput();

    //viterbi();
    return 0;
}

void get_data() {
    ifstream fb;
    fb.open("bigram.txt", ios::in);
    string tmpstr, buf, a1, a2, v;

    while (!fb.eof()) {
        getline(fb, tmpstr);
        stringstream ss(tmpstr);
        ss >> a1;
        ss >> a2;
        ss >> v;

        //cout << stod(v) << endl;
        // TODO deal zero_2
        bigram.insert(make_pair(make_pair(a1, a2), stod(v)));

        //bigram.insert(make_pair(make_pair))
    }
    fb.close();

    fb.open("dictionary.txt", ios::in);
    getline(fb, tmpstr); //<s> sil
    vector<string> seq;

    while (!fb.eof()) {
        getline(fb, tmpstr);
        stringstream ss(tmpstr);
        ss >> a1;
        while (ss >> a2) {
            seq.push_back(a2);
        }
        if(dictionary.find(a1)!=dictionary.end()){
            a1=a1+"_2";
        }
        dictionary.insert(make_pair(a1, seq));
    }
    fb.close();

    fb.open("./unigram", ios::in);
    while (!fb.eof()) {
        getline(fb, tmpstr);
        stringstream ss(tmpstr);
        ss >> a1;
        ss >> v;
        unigram.insert(make_pair(a1, stod(v)));
    }
    fb.close();
    return;
}

void tstinput() {
    ifstream fb;
    fb.open("./filename.txt", ios::in);
    string filepath;
    vector<string > result;

    while (!fb.eof()) {
        getline(fb, filepath);
        result=datafromtst(filepath);
        for(int i=0;i<result.size();i++)
            cout << result[i] << endl;
    }
    return;
}

vector<string > datafromtst(string path) {
    ifstream fb;
    double t;
    int n;
    string buf, a;

    vector<vector<double>> data;

    fb.open(path, ios::in);
    getline(fb, buf);
    stringstream ss(buf);
    ss >> a;
    ss >> a;
    n = stoi(a);

    while (!fb.eof()) {
        vector<double> l;
        getline(fb, buf);
        stringstream ss(buf);
        for (int i = 0; i < n; i++) {
            ss >> a;
            l.push_back(stod(a));
        }
        data.push_back(l);
    }


    int data_size = data.size();

    vector<double> prev(state_size, 0);
    vector<double> logdel(state_size, 0);
    double max = -9999999999, max_p,tmp_p;
    int psai[data_size + 5][state_size + 5],save_i;

    //viterbi algorithm

    for (int i = 0; i < state_size; i++) {
        prev[i] = stateprob(data[0], i)+hmm_tp[0][1+i];
    }

    for (int i = 1; i < data_size; i++) {
        for (int j = 0; j < state_size; j++) {
            for (int l = 0; l < state_size; l++) {
                max_p = max;
                tmp_p = prev[l] + hmm_tp[l][j];
                if (tmp_p > max_p) {
                    max_p = tmp_p;
                    save_i = i;
                }
            }
            logdel[j] = max_p+stateprob(data[i],j);
            psai[i][j] = save_i;
        }
        prev = logdel;
    }

    max_p=max;
    for (int i = 0; i < state_size; i++) {
        tmp_p = prev[i];
        if (tmp_p > max_p) {
            max_p = tmp_p;
            save_i = i;
        }
    }


    std::vector<int> result_states;
    int prev_i = save_i;
    int wordLen = 0;
    for (int t = data_size - 2; t >= 0; t--) {
        int i = psai[t + 1][prev_i];
        result_states.push_back(i);
        prev_i = i;
    }

    reverse(result_states.begin(), result_states.end());
    std::vector<std::string> result;
    // TODO make word boundary list
    return result;
}

double stateprob(vector<double> data_seg, int k) {
    stateType sT=states[k]
    vector<double> t(N_PDF);
    double tmp, max, res = 0;
    max = min_double;
    for (int i = 0; i < N_PDF; i++) {
        tmp = 0;
        for (int j = 0; j < N_DIMENSION; j++) {
            tmp += pow(data_seg[j] - sT.pdf[i].mean[j], 2) / sT.pdf[i].var[j];
        }
        tmp = pdfsum[k][i] - tmp / 2;
        if (tmp > max) {
            max = tmp;
        }
        t.push_back(tmp);
    }

    for (int i = 0; i < N_PDF; i++) {
        res += exp(t[i] - max);
    }
    res = max + log(res);
    return res;
}

void prev_pdfsum() {        // gausian mixture model
    double sum;
    //hmmType silphone=phones[syl_N-2];
    for (int i = 0; i < state_size; i++) {
        for (int j = 0; j < N_PDF; j++) {
            sum=0;
            for (int k = 0; k < N_DIMENSION; k++) {
                sum += log(2 * M_PI * states[i].pdf[j].var[k]);
            }
            pdfsum[i].push_back(log(states[i].pdf[j].weight)-sum/2);
        }
    }
}



void make_hmm(){
    //int diction[13]={0,};//8(2) 5(3) 4(3) 9(3) oh(1) one(3) 7(5) six(4) 3(3) 2(2) zero(4) zero(4)
    //hmm_tp[0][1]=;
    vec_phone();
    map< string, int > diction;
    int k=0,j=0;
    for (auto i=dictionary.begin();i!=dictionary.end();i++){
        //hmm_tp[0][1+k]=unigram[i->first];       //overwrite -> not use
        diction.insert(make_pair(i->first, 1+k));
        k+=(i->second.size()-1)*3;

    }
    diction.insert(make_pair("zero_2",1+k));
    k+=4*3;
    diction.insert(make_pair("sil",1+k));//[12]=1+k;    //sil
    //state_size=1+k+2;
    int head,tail,head2;
    string from,to;
    for(auto b=bigram.begin();b!=bigram.end();b++){
        from=b->first.first;
        to=b->first.second;
        if(from.compare("<s>")==0){
            if(to.compare("zero")==0)
                hmm_tp[0][diction["zero_2"]]=b->second;
            hmm_tp[0][diction[to]]=b->second;
        }
        else if(to.compare("<s>")==0){
            head=diction[from];
            tail=diction["sil"];
            if(from.compare("zero")==0) {
                head2 = diction["zero_2"];
                hmm_tp[head2 + 3 * (dictionary[from].size() - 1) - 1][tail] = b->second;
            }
            hmm_tp[head+3*(dictionary[from].size()-1)-1][tail]=b->second;
        }
        else {
            head = diction[from];
            tail = diction[to];
            for(int i=0;i<3;i++){
                for(int j=0;j<3;j++){
                    hmm_tp[head+i][head+j]=phone2[dictionary[from][0]].tp[i+1][j+1];
                }
            }
            hmm_tp[head+3*(dictionary[from].size()-1)-1][tail] = b->second;
            if(from.compare("zero")==0)
                hmm_tp[head+3*(dictionary["zero_2"].size()-1)-1][tail] = b->second;
            if(to.compare("zero")==0)
                hmm_tp[head+3*(dictionary[from].size()-1)-1][diction["zero_2"]] = b->second;
        }

    }
    head=diction["zero_2"];
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            hmm_tp[head+i][head+j]=phone2["z"].tp[i+1][j+1];
        }
    }


}

void vec_phone(){
    auto x=sizeof(char *);

    for(int i=0;i<syl_N;i++){
        hmmType2 *tmp=(hmmType2 *)(&phones[i]+x);
        phone2.insert(make_pair(phones[i].name,tmp));
    }
    for(int i=0;i<syl_N-1;i++){
        for(int j=0;j<3;j++)
            states.push_back(phones[i].state[j]);
    }
    states.push_back(phones[syl_N-1].state[0]); //sp
    state_size=states.size();
}