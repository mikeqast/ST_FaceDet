#include "DataProcessing.h"

bool DataProcessor::checkNewID(int id)
{
	if(crowd.find(id)==crowd.end())
		return true;//new person

	else
		return false;
	
}

void DataProcessor::addCluster(int intID, Impression imp)
{
	Cluster temp;
	crowd.insert(std::pair<int,Cluster>(intID, temp));
	crowd.at(intID).findIndex(imp, intID);
}
void DataProcessor::updateClusters()
{
	listToSend.clear();
	for (std::map<int,Cluster>::iterator it=crowd.begin(); it!=crowd.end(); ++it)
	{
		for (std::map<int,STPerson>::iterator it2=it->second.group.begin(); it2!=it->second.group.end(); ++it2)
		{
			if(it2->first>=0)
			{
				it2->second.frameUpdate();
				if(it2->second.needToShip)
				{
					listToSend.push_back(it2->second);
					it2->second.needToShip = false;
				}
			}
		}
	}
}