#include "MyUtils.h"
#include <sstream>
#include <string>
using namespace std;
MyUtils::MyUtils()
{
}

MyUtils::~MyUtils()
{
}

int MyUtils::string2int(string str)
{
     std::stringstream ss;
    ss << str;
    int c;
    ss >> c;
    return c;
}
double MyUtils::string2double(string str)
{
       std::stringstream ss;
    ss << str;
    double c;
    ss >> c;
    return c;
}
string MyUtils::int2string(int c)
{
       string str;
       stringstream ss;
       ss << c;
       ss >> str;
       return str;
}
//input: string, output: vector string divided by character "c"
void MyUtils::splitString(const std::string &s, std::vector<std::string> &v, const std::string &c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
      v.push_back(s.substr(pos1, pos2-pos1));

      pos1 = pos2 + c.size();
      pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
      v.push_back(s.substr(pos1));
}
