#include "Cluster.h"

Cluster::Cluster()
{

}
void Cluster::initParams()
{
}

//creates an index to place impressions into a map based on gender, age, and position in frame. 
int Cluster::findIndex(Impression data, int dbgid)
{
	//other than gender all values are currently collapsed into "buckets" to account for slight variations of each impression to go to the same place
	int returnIndex = 0;
	if(data.gender<=0)
	{
		returnIndex = -10000000;
	}
	else
	{
		returnIndex = 10000000;
	}
	returnIndex*=(int)(data.age/5);

	returnIndex += (int)(data.pos.x/(float)(WINDOWSIZE_X/SUBDIV_X))*1000;
	returnIndex += (int)(data.pos.y/(float)(WINDOWSIZE_Y/SUBDIV_Y));

	if(checkNewIndex(returnIndex))
	{
		if(CONFIG_MODE==1)
			std::cout<<"NEW PERSON of ID/INDEX:"<<dbgid<<"/"<<returnIndex<<"\n";
		addPerson(dbgid, returnIndex, data);
	}
	else
	{
		if(CONFIG_MODE==1)
			std::cout<<"NEW IMPRESSION for ID/INDEX:"<<dbgid<<"/"<<returnIndex<<"\n";
		group.at(returnIndex).addImpression(data, returnIndex);
	}
	return returnIndex;
}
void Cluster::addPerson(int id, int index, Impression imp)
{
	STPerson temp;
	temp.clusterID = id;
	temp.index = index;
	group.insert(std::pair<int,STPerson>(index, temp));
	group.at(index).addImpression(imp, id);
}

bool Cluster::checkNewIndex(int id)
{
	if(group.find(id)==group.end())
		return true;//new person
	else
		return false;
	
}