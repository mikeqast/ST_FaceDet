#ifndef ST_CLUSTER
#define ST_CLUSTER


#include "iniReader.h"
#include "STPerson.h"
extern int SUBDIV_X, SUBDIV_Y, WINDOWSIZE_X, WINDOWSIZE_Y, CONFIG_MODE;

typedef std::map<int, STPerson> UniqueData;

class Cluster
{
public:
	Cluster();
	UniqueData group;
	void initParams();
	int findIndex(Impression data, int dbgid);//this function creates a single int key to index into a map based on gender, age, and postion of impression
	void addPerson(int id, int index, Impression imp);
	void updatePeople();
	bool checkNewIndex(int index);
private:
	float ageSpread, genderSpread, posSpread, timeSpread;//values for determining how distinct something must be before counting as a new person
};

#endif