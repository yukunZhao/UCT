#pragma once
#include "Document.h"
#include <vector>
using namespace std;
class User
{
private:
	vector<WordPair> userWordPairSet;
	vector<Document> documents;
public:
	User();
	void setDocuments(vector<Document>&);
	void addDocument(Document&);
	void addWordPair(WordPair&);
	vector<WordPair>& getUserWordPairSet();
	int getUserWordPairCount();
	~User();
};

