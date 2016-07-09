#pragma once
#include "User.h"
#include <map>
using namespace std;
class Dataset
{
public:
	vector<User> users;
	int M; //# users
	int V;  //# words
	map<string, int> word2id;
	map<int, string> id2word;

	Dataset();
	void addUser(User&);
	
	void readTrainData(string dfile, string wordMapFile, string alphaFile, string betaFile);

	~Dataset();
};

