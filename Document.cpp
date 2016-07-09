#include "Document.h"


Document::Document()
{
}
Document::Document(vector<WordPair> &initialWordPair)
{
	wordPairSet.clear();
	for (int i = 0; i < initialWordPair.size(); i++)
	{
		wordPairSet.push_back(initialWordPair.at(i));
	}
}
void Document::setWordPairSet(vector<WordPair> &initialWordPair)
{
	wordPairSet.clear();
	for (int i = 0; i < initialWordPair.size(); i++)
	{
		wordPairSet.push_back(initialWordPair.at(i));
	}
}
void Document::addWordPair(WordPair &wpIni)
{
	WordPair wpIniTmp(wpIni.getWord1(), wpIni.getWord2());
	wordPairSet.push_back(wpIniTmp);
}
vector<WordPair>& Document::getWordPairSet()
{
	return wordPairSet;
}
Document::~Document()
{
	vector<WordPair>().swap(wordPairSet);
}
