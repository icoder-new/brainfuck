/*
* Simple interpreter brainfuck on Cpp (C++)
* Coded by Ehsonjon (a.k.a iCoder)
* DATE: 23.07.2020 (18:03) Tajikistan Asia Time =D
* (C) 2020-2021 All rights reversed!
*/

#include <iostream> // for cout and cin !
#include <fstream> // for ifstream
#include <vector> // for our brainfuck code =D

using namespace std;

static char cpu[30000];

int main(int argc, char **argv) {
    vector<char> acc;
    char ch;
    ifstream infile(argv[1]);
    while (infile) {
        infile.get(ch);
        acc.push_back(ch);
    }
    infile.close();
    unsigned int j = 0;
    int brc = 0;
    for (int i = 0; i < acc.size(); ++i) {
        if (acc[i] == '>')
            j++;
        if (acc[i] == '<')
            j--;
        if (acc[i] == '+')
            cpu[j]++;
        if (acc[i] == '-')
            cpu[j]--;
        if (acc[i] == '.')
            cout << cpu[j];
        if (acc[i] == ',')
            cin >> cpu[j];
        if (acc[i] == '[') {
            if (!cpu[j]) {
                ++brc;
                while (brc) {
                    ++i;
                    if (acc[i] == '[')
                        ++brc;
                    if (acc[i] == ']')
                        --brc;
                }
            } else
                continue;
        } else if (acc[i] == ']') {
            if (!cpu[j])
                continue;
            else {
                if (acc[i] == ']')
                    brc++;
                while (brc) {
                    --i;
                    if (acc[i] == '[')
                        brc--;
                    if (acc[i] == ']')
                        brc++;
                }
                --i;
            }
        }
    }
}