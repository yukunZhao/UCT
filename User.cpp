#include "User.h"
#include "MyUtils.h"

User::User()
{
}

void User::setDocuments(vector<Document>& docsIni)
{
	documents.clear();
	userWordPairSet.clear();
	for (int i = 0; i < docsIni.size(); i++)
	{
		addDocument(docsIni.at(i));
	}
}
void User::addDocument(Document& doc)
{
	documents.push_back(doc);
	for (int i = 0; i < doc.wordPairSet.size(); i++)
	{
		addWordPair(doc.wordPairSet.at(i));
	}
}
void User::addWordPair(WordPair& wordPair)
{
	userWordPairSet.push_back(wordPair);
}

vector<WordPair>& User::getUserWordPairSet()
{
	return userWordPairSet;
}
int User::getUserWordPairCount()
{
       return userWordPairSet.size();
}
User::~User()
{
	vector<WordPair>().swap(userWordPairSet);
}
