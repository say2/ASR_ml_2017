//
// Created by 김세희 on 2017. 12. 8..
//

#include "hmm.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>

using namespace std;
#define _USE_MATH_DEFINES
float min_float = -9999999999;
int state_size;
int syl_N = sizeof(phones) / sizeof(hmmType);
string dic[13] = {"eight", "five", "four", "nine", "oh", "one", "seven", "six", "three", "two", "zero", "zero_2",
                  "<s>"};
map<string, int> diction;

map<pair<string, string>, float> bigram;
map<string, vector<string> > dictionary;
map<string, hmmType2> phone2;
vector<stateType> states;
vector<vector<float> > prepdfsum(1000);
float hmm_tp[1000][1000];

void get_data();

vector<string> datafromtst(string path);

void tstinput();

float stateprob(vector<float>, int);

void make_hmm();

void vec_phone();

void prev_pdfsum();

int main() {
    get_data();
    make_hmm();
    prev_pdfsum();
    tstinput();
    return 0;
}

void get_data() {
    ifstream fb;
    fb.open("./bigram.txt", ios::in);
    string tmpstr, buf, a1, a2, v;

    while (!fb.eof()) {
        getline(fb, tmpstr);
        stringstream ss(tmpstr);
        ss >> a1;
        ss >> a2;
        ss >> v;

        bigram.insert(make_pair(make_pair(a1, a2), stod(v)));

    }
    fb.close();

    fb.open("./dictionary.txt", ios::in);

    while (!fb.eof()) {
        vector<string> seq;
        getline(fb, tmpstr);
        stringstream ss(tmpstr);
        ss >> a1;
        while (ss >> a2) {
            seq.push_back(a2);
        }
        if (dictionary.find(a1) != dictionary.end()) {
            a1 = a1 + "_2";
        }
        dictionary.insert(make_pair(a1, seq));
    }
    fb.close();
}

void tstinput() {
    ifstream fb;
    fb.open("./filename.txt", ios::in);
    string filepath;
    vector<string> result;
    ofstream fw;
    fw.open("./result.txt",ios::out);
    fw << "#!MLF!#" << endl;
    while (!fb.eof()) {
        getline(fb, filepath);
        result = datafromtst(filepath);
        fw << "\"" << filepath.substr(2,filepath.length()-4) << ".lab\"" << endl;
        cout << filepath << endl;
        for (int i = 0; i < result.size()-1; i++)
            fw << result[i] << endl;
        fw << "." << endl;
    }
    return;
}

vector<string> datafromtst(string path) {
    ifstream fb;
    float t;
    int n;
    string buf, a;

    vector<vector<float>> data;

    fb.open(path, ios::in);
    getline(fb, buf);
    stringstream ss(buf);
    ss >> a;
    ss >> a;
    n = stoi(a);

    while (!fb.eof()) {
        vector<float> l;
        getline(fb, buf);
        stringstream ss(buf);
        for (int i = 0; i < n; i++) {
            ss >> a;
            l.push_back(stod(a));
        }
        data.push_back(l);
    }


    int data_size = data.size();

    vector<float> prev(state_size, 0);
    vector<float> logdel(state_size, 0);
    float max = -9999999, max_p, tmp_p;
    int psai[1000][1000];//[data_size + 5][state_size + 5];
    int save_ind;

    //viterbi algorithm

    for (int i = 0; i < state_size; i++) {
        prev[i] = stateprob(data[0], i) + hmm_tp[0][1 + i];
    }

    for (int i = 1; i < data_size; i++) {
        for (int j = 0; j < state_size; j++) {
            max_p = max;
            for (int l = 0; l < state_size; l++) {
                if(hmm_tp[l][j]==0)
                    continue;
                tmp_p = prev[l] + hmm_tp[l + 1][j + 1];
                if (tmp_p > max_p) {
                    max_p = tmp_p;
                    save_ind = l;
                }
            }
            logdel[j] = max_p + stateprob(data[i], j);
            psai[i][j] = save_ind;
        }
        prev = logdel;
    }

    max_p = max;
    for (int i = 0; i < state_size; i++) {
        tmp_p = prev[i];
        if (tmp_p > max_p) {
            max_p = tmp_p;
            save_ind = i;
        }
    }


    std::vector<int> result_states;
    int prev_i = save_ind;
    for (int k = data_size - 1; k > 0; k--) {
        int i = psai[k][prev_i];
        result_states.push_back(i);
        prev_i = i;
    }

    reverse(result_states.begin(), result_states.end());
    std::vector<std::string> result;

    bool check = 0;
    string w;
    for (int i = 0; i < result_states.size(); i++) {
        int x=result_states[i];
        if (!check) {
            for (auto j = diction.begin(); j != diction.end(); j++) {
                if (result_states[i] == j->second && result_states[i] != result_states[i + 1]) {
                    check = 1;
                    result.push_back(j->first);
                    w = j->first;
                    break;
                }
            }

        }
        else if (result_states[i] == diction.upper_bound(w)->second - 1) {
            check = 0;
        }


    }
    return result;
}

float stateprob(vector<float> data_seg, int k) {
    stateType sT = states[k];
    vector<float> t;
    float tmp, max, res = 0;
    max = min_float;
    for (int i = 0; i < N_PDF; i++) {
        tmp = 0;
        for (int j = 0; j < N_DIMENSION; j++) {
            tmp += pow(data_seg[j] - sT.pdf[i].mean[j], 2) / sT.pdf[i].var[j];
        }
        tmp = prepdfsum[k][i] - tmp / 2;
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
    float sum;

    for (int i = 0; i < state_size; i++) {
        vector<float> tmp;
        for (int j = 0; j < N_PDF; j++) {
            sum = 0;
            for (int k = 0; k < N_DIMENSION; k++) {
                float x = states[i].pdf[j].var[k];
                sum += log(2.0f * M_PI * states[i].pdf[j].var[k]);
                //cout << sum << endl;
            }
            tmp.push_back(log(states[i].pdf[j].weight) - sum / 2);
        }
        prepdfsum[i] = tmp;
    }
}


void make_hmm() {
    vec_phone();

    int k = 0, j = 0;
    for (auto i = dictionary.begin(); i != dictionary.end(); i++) {
        if (i->first == "<s>")
            continue;
        string s = i->first;
        diction.insert(make_pair(i->first, 1 + k));
        k += (i->second.size() - 1) * 3 + 1;    //for sp
    }
    diction.insert(make_pair("<s>", 1 + k));//[12]=1+k;    //sil

    int head, tail,  h, l;
    string from, to;
    for (int i = 0; i < 12; i++) {
        int c = 0;
        from = dic[i];
        head = diction[from];
        for (h = 0; h < dictionary[from].size() - 1; h++) {
            for (l = 0; l < 3; l++) {
                for (j = 0; j < 4; j++) {
                    hmm_tp[head + 3 * h + l][head + 3 * h + j] = phone2[dictionary[from][c]].tp[l + 1][j + 1];
                }
            }
            c++;
        }
        hmm_tp[head + 3 * h][head + 3 * h] = phone2["sp"].tp[1][1];
    }
    head = diction["<s>"];
    for (l = 0; l < 3; l++) {
        for (j = 0; j < 3; j++) {
            hmm_tp[head + l][head + j] = phone2["sil"].tp[l + 1][j + 1];
        }
    }
    hmm_tp[head + l][0] = phone2["sil"].tp[3][4];

    for (auto b = bigram.begin(); b != bigram.end(); b++) {
        from = b->first.first;
        to = b->first.second;
        if (from.compare("<s>") == 0) {
            if (to.compare("zero") == 0)
                hmm_tp[0][diction["zero_2"]] = b->second;
            hmm_tp[0][diction[to]] = b->second;
        } else if (to.compare("<s>") == 0) {
            head = diction[from];
            tail = diction["<s>"];
            if (from.compare("zero") == 0) {
                hmm_tp[diction.upper_bound("zero_2")->second - 1][tail] = b->second;
            }
            hmm_tp[diction.upper_bound(from)->second - 1][tail] = b->second;
        } else {
            head = diction[from];
            tail = diction[to];


            hmm_tp[diction.upper_bound(from)->second - 1][tail] = b->second;
            if (from.compare("zero") == 0)
                hmm_tp[diction.upper_bound("zero_2")->second - 1][tail] = b->second;
            if (to.compare("zero") == 0)
                hmm_tp[diction.upper_bound(from)->second - 1][diction["zero_2"]] = b->second;
        }

    }
}

void vec_phone() {
    auto x = sizeof(char *);

    for (int i = 0; i < syl_N; i++) {
        hmmType2 *tmp;
        tmp = (hmmType2 *) malloc(sizeof(hmmType2));
        memcpy(tmp->state, phones[i].state, sizeof(stateType) * N_STATE);
        memcpy(tmp->tp, phones[i].tp, sizeof(phones[i].tp));
        phone2.insert(make_pair(phones[i].name, *tmp));
        free(tmp);
    }
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < dictionary[dic[i]].size(); j++) {
            if (dictionary[dic[i]][j].compare("sp") == 0) {
                states.push_back(phone2[dictionary[dic[i]][j]].state[0]);
            } else {
                for (int l = 0; l < 3; l++)
                    states.push_back(phone2[dictionary[dic[i]][j]].state[l]);
            }
        }
    }
    state_size = states.size();
}