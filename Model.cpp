#include "Model.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <cstdlib>
#include "WordPair.h"
#include "Digamma.h"
using namespace std;

Model::Model(int argc, char **argv)
{
       parseArgs(argc, argv);
       readTrainData();
       setDefaultValues();
       if (!saveModel_wordMap())
       {
              cout << "save wordMap file erro.." << endl;
              return;
       }
       init();
}
Model::~Model()
{
}
void Model::saveModel()
{
       if (!saveModel_theta())
       {
              cout << "save theta file erro.." << endl;
              return;
       }
       if (!saveModel_phi())
       {
              cout << "save phi file erro.." << endl;
              return;
       }

}
bool Model::saveModel_wordMap()
{
       string fileName = model_name + save_model_wordMap_suffix;
       ofstream out(fileName.c_str());
       if (out.is_open())
       {
              map<int, string>::iterator itWordMap = id2word.begin();
              for (itWordMap; itWordMap != id2word.end(); itWordMap++)
              {
                     out << itWordMap->first << " " << itWordMap->second << endl;
              }
              out.close();
              return true;
       }
       return false;
}
bool Model::saveModel_theta()
{
       string fileName = model_name + save_model_theta_suffix;
       ofstream out(fileName.c_str());
       if (out.is_open())
       {
              for (int my_rank = 0; my_rank < M; my_rank++)
              {
                     double norm = 0.0;
                     for (int k_rank = 0; k_rank < K; k_rank++)
                     {
                            norm += theta[my_rank][k_rank];
                     }
                     for (int k_rank = 0; k_rank < K; k_rank++)
                     {
                            theta[my_rank][k_rank] = theta[my_rank][k_rank] / norm;
                            out << theta[my_rank][k_rank] << " ";
                     }
                     out << endl;
              }
              out.close();
              return true;
       }
       return false;
}
bool Model::saveModel_phi()
{
       string fileName = model_name + save_model_phi_suffix;
       ofstream out(fileName.c_str());
       if (out.is_open())
       {
              for (int v_rank = 0; v_rank < V; v_rank++)
              {
                     double norm = 0.0;
                     for (int k_rank = 0; k_rank < K; k_rank++)
                     {
                            norm += phi[v_rank][k_rank];
                     }
                     for (int k_rank = 0; k_rank < K; k_rank++)
                     {
                            phi[v_rank][k_rank] = phi[v_rank][k_rank] / norm;
                            out << phi[v_rank][k_rank] << " ";
                     }
                     out << endl;
              }
              out.close();
              return true;
       }
       return false;
}
void Model::estimate()
{
       cout << "estimate begin, K=" << K << ", niter=" << niters << endl;
       for (int my_rank = 1; my_rank < niters + 1; my_rank++)
       {
              if (my_rank % 10 == 0)
              {
                     cout << "Iteration " << my_rank << "." << endl;
              }
              for (int m = 0; m < M; m++)
              {
                     vector<WordPair> userWordPairTmp = users[m].getUserWordPairSet();
                     int userWordPairCount = users[m].getUserWordPairCount();
                     for (int b = 0; b < userWordPairCount; b++)
                     {
                            int topic = sample(m, b, userWordPairTmp);
                            z[m][b] = topic;
                     }
              }
              updateAlpha();
              updateBeta();
       }//iteration finished.
       saveModel();
}
int Model::sample(int userIndex, int wordPairIndex, vector<WordPair> &userWordPairTmp)
{
       int topic = z[userIndex][wordPairIndex];
       map<string, int>::iterator itWord1 = word2id.find(userWordPairTmp[wordPairIndex].getWord1());
       map<string, int>::iterator itWord2 = word2id.find(userWordPairTmp[wordPairIndex].getWord2());
       nd[userIndex][topic] -= 1;
       if (itWord1 != word2id.end() && itWord2 != word2id.end())
       {
              nw[itWord1->second][topic] -= 1;
              nw[itWord2->second][topic] -= 1;
       }
       double KAlpha = 0.0;
       double VBeta = 0.0;

       for (int i = 0; i < K; i++)
       {
              KAlpha += alpha[i];
       }
       for (int i = 0; i < V; i++)
       {
              VBeta += beta[i];
       }
       double maxValue = 0.0;
       int maxIndex = 0;
       for (int k_rank = 0; k_rank < K; k_rank++)
       {
            p[k_rank] = ( (nw[itWord1->second][k_rank] + phiPre[itWord1->second][k_rank]+ beta[itWord1->second] -1)
                            / (nwsum[k_rank] + VBeta + phiPreSum[k_rank] - 1) ) *
                     ( (nw[itWord2->second][k_rank] + phiPre[itWord2->second][k_rank]+ beta[itWord2->second] -1)
                     / (nwsum[k_rank] + VBeta + phiPreSum[k_rank] - 1) )  *
                     ((nd[userIndex][k_rank] + thetaPre[userIndex][k_rank] + alpha[k_rank])
                     / (ndsum[userIndex] + KAlpha  -1));
              //test code
              /*cout << (nw[itWord1->second][k_rank] + phiPre[itWord1->second][k_rank]+ beta[itWord1->second] -1) << " "
               << (nw[itWord2->second][k_rank] + phiPre[itWord2->second][k_rank]+ beta[itWord2->second] -1) << " "
               << (nwsum[k_rank] + VBeta + phiPreSum[k_rank] - 1) << " "
               << (nd[userIndex][k_rank] + thetaPre[userIndex][k_rank] + alpha[k_rank]) << " "
               << (ndsum[userIndex] + KAlpha  -1) << endl;
              cout << "p[k_rank] " << p[k_rank] << endl;*/
              if (p[k_rank] > maxValue)
              {
                     maxValue = p[k_rank];
                     maxIndex = k_rank;
              }
       }
       return maxIndex;

}
void Model::updateAlpha()
{
       for (int my_rank = 0; my_rank < K; my_rank++)
       {
              double sumTmp1 = 0, sumTmp2 = 0;
              double denTmp = 0;
              for (int i = 0; i < M; i++)
              {
                     sumTmp1 += Digamma::digammal(nd[i][my_rank] + thetaPre[i][my_rank] + alpha[my_rank] -1);
                     sumTmp2 += Digamma::digammal(thetaPre[i][my_rank] + alpha[my_rank] );
                     double sumDeTmp1 = 0, sumDeTmp2 = 0;
                     for (int z_rank = 0; z_rank < K; z_rank++)
                     {
                            sumDeTmp1 += nd[i][z_rank] + thetaPre[i][z_rank] + alpha[z_rank] -1;
                            sumDeTmp2 += thetaPre[i][z_rank] + alpha[z_rank] ;
                     }
                     denTmp += Digamma::digammal(sumDeTmp1) - Digamma::digammal(sumDeTmp2);
              }

              alpha[my_rank] = alpha[my_rank] * ( (sumTmp1 - sumTmp2 )/denTmp  );

       }

}
void Model::updateBeta()
{
       for (int my_rank = 0; my_rank < V; my_rank ++)
       {
              double sumTmp1 = 0, sumTmp2 = 0;
              double denTmp = 0;
              for (int z_rank = 0; z_rank < K; z_rank++)
              {
                     sumTmp1 += Digamma::digammal(nw[my_rank][z_rank] + phiPre[my_rank][z_rank] + beta[my_rank] -1);
                     sumTmp2 += Digamma::digammal(phiPre[my_rank][z_rank] + beta[my_rank]);
                     double sumDenTmp1 = 0, sumDenTmp2 = 0;
                     for (int v_rank = 0; v_rank < V; v_rank++)
                     {
                            sumDenTmp1 += nw[v_rank][z_rank] + phiPre[v_rank][z_rank] + beta[v_rank] -1;
                            sumDenTmp2 += phiPre[v_rank][z_rank] + beta[v_rank];
                     }
                     denTmp += Digamma::digammal(sumDenTmp1) - Digamma::digammal(sumDenTmp2);
              }
              beta[my_rank] = abs(beta[my_rank] * ( (sumTmp1 - sumTmp2) / denTmp));
       }
}
void Model::init()
{
       alpha = new double[K];
       for (int i = 0; i < K; i++)
       {
              alpha[i] = 0.1;
       }
       beta = new double[V];
       for (int i = 0; i < V; i++)
       {
              beta[i] = 0.1;
       }
       p = new double[K];

       nd = new int*[M];
       for (int m = 0; m < M; m++)
       {
              nd[m] = new int[K];
              for (int k = 0; k < K; k++)
              {
                     nd[m][k] = 0;
              }
       }

       nw = new int*[V];
       for (int w = 0; w < V; w++)
       {
              nw[w] = new int[K];
              for (int k = 0; k < K; k++)
              {
                     nw[w][k] = 0;
              }
       }
       ndsum = new int[M];
       for (int i = 0; i < M; i++)
       {
              ndsum[i] = 0;
       }
       nwsum = new int[K];
       for (int i = 0; i < K; i++)
       {
              nwsum[i] = 0;
       }
       srand(time(0));
       z = new int*[M];
       for (int m = 0; m < M; m++)
       {
              vector<WordPair> userWordPairTmp = users[m].getUserWordPairSet();
              int userWordPairCount = users[m].getUserWordPairCount();
              z[m] = new int[userWordPairCount];
              for (int b = 0; b < userWordPairCount; b++)
              {

                     int topic = (int)(((double)rand() / RAND_MAX) * K);
                     z[m][b] = topic;
                     map<string, int>::iterator itWord1 = word2id.find(userWordPairTmp[b].getWord1());
                     map<string, int>::iterator itWord2 = word2id.find(userWordPairTmp[b].getWord2());
                     if (itWord1 != word2id.end() && itWord2 != word2id.end())
                     {
                            nw[itWord1->second][topic] += 1;
                            nw[itWord2->second][topic] += 1;
                     }
                     nd[m][topic] += 1;
                     nwsum[topic] += 2;
                     ndsum[m] += 1;
              }
       }

}
void Model::setDefaultValues()
{
       model_name = inputDataFile + ".k" + MyUtils::int2string(K) + ".iter" + MyUtils::int2string(niters);
       save_model_wordMap_suffix = ".wordMap";
       save_model_phi_suffix = ".phi";
       save_model_theta_suffix = ".theta";
}
void Model::parseArgs(int argc, char **argv)
{
       int i = 0;
       while (i < argc)
       {
              string argvTmp = argv[i];
              if (argvTmp == "-dataFile")
               inputDataFile = argv[++i];
              else if (argvTmp == "-thetaFile")
                     thetaPreFile = argv[++i];
              else if (argvTmp == "-phiFile")
                     phiPreFile = argv[++i];
              else if (argvTmp == "-nTopics")
              {
                     string temp = argv[++i];
                     K = MyUtils::string2int(temp);
              }
              else if (argvTmp == "-nIters")
              {
                     string temp = argv[++i];
                     niters = MyUtils::string2int(temp);
              }
              else{}
              i++;

       }
}
void Model::readTrainData()
{
       if(!readWordPair())
              return;
       if (!readPreTheta())
              return;
       if(!readPrePhi())
              return;

}
bool Model::readWordPair()
{
    ifstream openFile(inputDataFile.c_str(), ios::in);
    if (openFile.fail())
    {
        printf("Cannot open file %s !\n", inputDataFile.c_str());
        return 0;
    }
    string strTmp;
    getline(openFile, strTmp, '\n');

    M = MyUtils::string2int(strTmp);
    if (M <= 0) {
	printf("No document available!\n");
	return 0;
    }
    // set number of words to zero
    V = 0;
    for (int i = 0; i < M; i++)
    {
        getline(openFile, strTmp, '\n');
        User userTmp;
        vector<string> bitermsTmp;
        MyUtils::splitString(strTmp, bitermsTmp, " ");
        for (int j =0; j <  bitermsTmp.size(); j++)
       {
              //a wordPair word1:word2
              WordPair wp(bitermsTmp.at(j)) ;
              userTmp.addWordPair(wp);
              map<string, int>::iterator itWord2id;
              itWord2id = word2id.find(wp.getWord1());
              if ( itWord2id == word2id.end() )
              {
                     word2id.insert(pair<string, int>(wp.getWord1(), V) ) ;
                     id2word.insert(pair<int, string>(V, wp.getWord1()) );
                     V++;
              }
              itWord2id = word2id.find(wp.getWord2());
               if ( itWord2id == word2id.end() )
              {
                     word2id.insert(pair<string, int>(wp.getWord2(), V) ) ;
                     id2word.insert(pair<int, string>(V, wp.getWord2()) );
                     V++;
              }
        }
        users.push_back(userTmp);
        }
        M=users.size();
        openFile.close();
        return 1;
}
//user-topic distribution, M*K
bool Model::readPreTheta()
{
       thetaPre = new double*[M];
       for (int i = 0; i < M; i++)
       {
              thetaPre[i] = new double[K];
              for (int j = 0; j < K; j++)
              {
                     thetaPre[i][j] = 1.0/(double)K;
              }
       }
       ifstream inStream(thetaPreFile.c_str(), ios::in);
       if (inStream.fail())
       {
              cout << "open file failed.." << endl;
              return false;
       }
       string line;
       int row = 0;
       while ( !inStream.eof())
       {
              getline(inStream, line, '\n');
              if (line != "")
              {
                     if (row < M)
                     {
                            vector<string> eleTmp;
                            MyUtils::splitString(line, eleTmp, " ");
                            for (int j = 0; j < eleTmp.size(); j++)
                            {
                                   thetaPre[row][j] = MyUtils::string2double(eleTmp[j]);
                            }
                     }
                     row++;
              }
       }
       inStream.close();
       return 1;
}
bool Model::readPrePhi()
{
       phiPreSum = new double[K];
       for (int j = 0; j < K; j++)
       {
              phiPreSum[j] = 0;
       }
       phiPre = new double*[V];
       for (int i = 0; i < V; i++)
       {
              phiPre[i] = new double[K];
              for (int j = 0; j < K; j++)
              {
                     phiPre[i][j] = 1.0/(double)K;
              }
       }
       ifstream inStream(phiPreFile.c_str(), ios::in);
        if (inStream.fail())
       {
              cout << "open file failed.." << endl;
              return false;
       }
       string line;
       int row = 0;
       while ( !inStream.eof())
       {
              getline(inStream, line, '\n');
              if (line != "")
              {
                     if (row < V)
                     {
                            vector<string> eleTmp;
                            MyUtils::splitString(line, eleTmp, " ");
                            for (int j = 0; j < eleTmp.size(); j++)
                            {
                                   phiPre[row][j] = MyUtils::string2double(eleTmp[j]);
                                   phiPreSum[j] += phiPre[row][j];
                            }
                     }
                     row++;
              }
       }
       inStream.close();
       return 1;
}
vector<User>& Model::getUsers(){return users;}
