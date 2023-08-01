#include "simnet/common/utils.h"
#include <iostream>
#include <map>
#include <fstream>
using namespace std;

int main()
{
    size_t jobId = 1;
    map<size_t, size_t> frequency;
    fstream datafile;
    datafile.open("data.txt", ios::out);
    for (size_t seqNumber = 1e3; seqNumber <= 1e10; seqNumber+= 1000) {
        std::size_t agtrIndex = seqNumber ^ jobId;
        hash_combine(agtrIndex, jobId);
        hash_combine(agtrIndex, seqNumber);
        agtrIndex %= 40000;
        if (frequency.find(agtrIndex) == frequency.end()) {
            frequency[agtrIndex] = 1;
        }
        else {
            frequency[agtrIndex] += 1;
        }
    }
    for (const auto& [index, freq]:frequency) {
        if (freq > 1) {
            datafile << index << " " << freq << endl;
        }
    }
    datafile.close();
}

