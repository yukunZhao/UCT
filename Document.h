#pragma once
#include "WordPair.h"
#include <vector>

using namespace std;
class Document
{
public:
	vector<WordPair> wordPairSet;
	//#words
	int length;
	Document();
	Document(vector<WordPair>&);
	void setWordPairSet(vector<WordPair>&);
	void addWordPair(WordPair&);
	vector<WordPair>& getWordPairSet();
	~Document();
};

