#include <string>
#include <vector>
using namespace std;
class MyUtils
{
    public:
        MyUtils();
        ~MyUtils();
        static int string2int(string);
        static double string2double(string);
        static string int2string(int);
        static void splitString(const std::string &s, std::vector<std::string> &v, const std::string &c);
    protected:
    private:
};
