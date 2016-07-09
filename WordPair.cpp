#include "WordPair.h"
#include "MyUtils.h"
#include <vector>
#include <string>
using namespace std;

WordPair::WordPair(const string& wordA, const string& wordB)
{
	word1 = wordA;
	word2 = wordB;
}
WordPair::WordPair(string &wpStr)
{
       string2WordPair(wpStr);
}
bool WordPair::string2WordPair(string &wordPair)
{
         vector<string> twoWords;
       MyUtils::splitString(wordPair, twoWords, ":");
       if (twoWords.size() == 2)
       {
           word1 = twoWords.at(0);
           word2 = twoWords.at(1);
           return 1;
       }
       return 0;
}
string WordPair::toString()
{
       return word1 + ":" + word2;
}
string WordPair::getWord1()
{
       return word1;
}
string WordPair::getWord2()
{
       return word2;
}
WordPair::~WordPair()
{
}
