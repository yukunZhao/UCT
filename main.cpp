#include <iostream>
#include <cmath>
#include "WordPair.h"
#include "Document.h"
#include "User.h"
#include "Model.h"
#include "Utils.h"
#include "Digamma.h"
using namespace std;

int main(int argc, char** argv)
{
       Model model(argc, argv);
       model.estimate();
       return 0;
}
