#ifndef ST_DATA_PROCESSING
#define ST_DATA_PROCESSING

#include "Cluster.h"
#include <map>
#include <vector>
#include "STPerson.h"


class DataProcessor
{
public:
	bool checkNewID(int id);
	
	void updateClusters();
	std::map<int, Cluster> crowd;
	std::vector<STPerson> listToSend;
	
	void addCluster(int intID, Impression tempImp);
private:

	
};

#endif