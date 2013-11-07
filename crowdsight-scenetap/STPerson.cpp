#include "STPerson.h"


STPerson::STPerson()
{
	impCount =0;
	lastImp=0;
	sentCount=0;
	activeState = 0;
	frequency = 0;
	dormant = false;
	needToShip = false;
}

void STPerson::changeState(int state)
{
	if(state>activeState)
	{
		if(activeState>SENT_THRESHOLD)
		{
			activeState = -10;
			return;
		}
		else
		{
			aggregateImpressions();
			needToShip=true;
		}
		
		
	}
	activeState = state;
}

void STPerson::addImpression(Impression data, int dbgid)
{
	++frequency;
	if(data.age<0||data.age>100||data.gender<-5||data.gender>5)
		return;//discard junk data, should probably cap and report instead...

	std::cout<<impCount;
	//if a single object has been reported too many times ever or over frequency threshold over the last 50 frames
	if(impCount>IMPRESSION_THRESHOLD_B||activeState < 0)
	{
		return;//TODO handle this case better
	}
	else if(impCount<IMPRESSION_THRESHOLD_A)
	{

		impressions.push_back(data);
	}
	else
	{
		impressions.push_back(data);
		impressions.pop_front();
		--frequency;
		
		if(impCount>(IMPRESSION_THRESHOLD_A*(activeState+1)))
			changeState(activeState+1);
	}
	if(CONFIG_MODE ==1)
		std::cout<<"Fully Added Impression for:"<<dbgid<<":"<<index<<"\n";
	dormant = false;
	++impCount;
	
	lastImp = 0;
}

void STPerson::aggregateImpressions()
{
	aveImpression.age = 0;
	aveImpression.gender = 0;
	for (std::list<Impression>::iterator it = impressions.begin(); it != impressions.end(); it++)
	{
		aveImpression.age += it->age;
		aveImpression.gender+= it->gender;
		
	}

	if(impressions.size()>0)
	{
		aveImpression.age /= impressions.size();
		aveImpression.gender /= impressions.size();
	}
	gender_s = "male";
	if(aveImpression.gender >= 0.0)
		gender_s = "female";
}

void STPerson::frameUpdate()
{

	if(frequency>0)
	{
		for (std::list<Impression>::iterator it = impressions.begin(); it != impressions.end(); it++)
		{
			--it->countdown;
			if(it->countdown<=0)
			{
				--frequency;
			}
		}
	}

	if(impressions.size()>0)
	{

	}

	if(activeState == -10||dormant)//locked out or inactive
	{
		return;
	}
	if(frequency>FREQUENCY_THRESHOLD)//occuring too often
	{
		if(CONFIG_MODE == 1)
			std::cout<<"Freq overload"<<index<<":"<<frequency<<"\n";
		changeState(-5);
		return;
	}
	if(lastImp>DORMANT_THRESHOLD)
	{
		dormant = true;
		changeState(activeState+1);
	}

	++lastImp;
}