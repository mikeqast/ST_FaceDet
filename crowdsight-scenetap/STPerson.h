#ifndef ST_PERSON
#define ST_PERSON

#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <opencv2/opencv.hpp>
extern int IMPRESSION_THRESHOLD_A, IMPRESSION_THRESHOLD_B, SENT_THRESHOLD, DORMANT_THRESHOLD, FREQUENCY_THRESHOLD, CONFIG_MODE;
struct Impression
{
	float age;
	float gender;
	cv::Point2d pos;
	int countdown;

};

class STPerson
{
public:
	STPerson();
	STPerson(Impression imp, int frame);
	void addImpression(Impression data, int dbgid);//add one capture to this ID
	void aggregateImpressions();
	void frameUpdate();//returns true to send data, false to not
	void changeState(int newState);//change state
	int impCount;//total number of impressions 
	int sentCount;//total number of times sent
	int lastImp;//how many frames have passed since last seen
	int activeState;
	int frequency;//how many times in the last 100 frames this object has been reported
	Impression aveImpression;
	bool needToShip;
	bool dormant;
	std::string gender_s;
	int clusterID, index; //clusterID refers to which id will access the proper cluster, index is the value inside the cluster
private:
	std::list<Impression> impressions;

};

#endif