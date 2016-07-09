#pragma once
#include "User.h"
#include "MyUtils.h"
#include <map>
using namespace std;
class Model
{
private:
	vector<User> users;
	map<int, string> id2word;
	map<string, int> word2id;
       string inputDataFile;
	string thetaPreFile;
	string phiPreFile;
	double **thetaPre; //estimated from previous time
	double **phiPre; //estimated from previous time
	double *thetaPreSum;
       double *phiPreSum;
	int M; //number of users
	int V; //number of words
	int K; //number of topics
	double *alpha; //alpha for current time
	double *beta;  //beta for current time
	int niters; //number of sampling iterations

	double *p;
	int ** z; //topic assignments on word-pairs, M * number of word-pairs
	int **nd;
	int *ndsum;
	int **nw;
	int *nwsum;
	double **theta;
	double **phi;
	string model_name;
	string save_model_wordMap_suffix;
	string save_model_phi_suffix;
	string save_model_theta_suffix;

	bool readWordPair();
	bool readPreTheta();
	bool readPrePhi();
	void readTrainData();
	void setDefaultValues();
	void parseArgs(int, char**);
	void init();
	int sample(int, int, vector<WordPair>&);
	void updateAlpha();
	void updateBeta();
	void computePhi();
	void computeTheta();
	void saveModel();
	bool saveModel_wordMap();
	bool saveModel_theta();
	bool saveModel_phi();

public:
	Model(int, char**);
	~Model();
       void estimate();

       vector<User>& getUsers();
	double** getThetaPre(){return thetaPre;}
	double** getPhiPre(){return phiPre;}

};

