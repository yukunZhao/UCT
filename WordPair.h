#pragma once
#include <string>
using namespace std;
class WordPair
{
private:
       string word1;
	string word2;
public:
	WordPair(const string&, const string&);
	WordPair(string&);
	string toString();
	bool string2WordPair(string&);
	string getWord1();
	string getWord2();
	~WordPair();
};

