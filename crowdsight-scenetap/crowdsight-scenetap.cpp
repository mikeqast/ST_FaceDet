// crowdsight-scenetap.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <csignal>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>

#include <list>


#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#endif

#define VERSION "0.1.1"


#include <opencv2/opencv.hpp>

#include "crowdsight.h"
#include "DataProcessing.h"
#include "CamFinder.h"

#include "iniReader.h"
#include <JsonBox.h>

#if defined( _MSC_VER )
#define snprintf _snprintf
#endif

#define HUMAN_NAME "WINDOW TITLE"

// Turn on/off recording of output video and csv
#define RECORDING 0

//Global variables for setting various threholds and other config parameters that apply to all systems
int IMPRESSION_THRESHOLD_A, IMPRESSION_THRESHOLD_B, SENT_THRESHOLD, DORMANT_THRESHOLD, FREQUENCY_THRESHOLD, SUBDIV_X, SUBDIV_Y, WINDOWSIZE_X, WINDOWSIZE_Y, CONFIG_MODE;

const FeaturesRequest ALL_FEATURES = { true, true, true, true, true, true, true, true, true };




#define SVCNAME TEXT("CrowdSightService")

SERVICE_STATUS          gSvcStatus; 
SERVICE_STATUS_HANDLE   gSvcStatusHandle; 
HANDLE                  ghSvcStopEvent = NULL;



namespace
{
	volatile sig_atomic_t running = 1;

	void signal_handler(int sig)
	{
		signal(sig, signal_handler);
		std::cout << "SIGNAL " << sig << " - Stopping" << std::endl;
		running = 0;
	}
}


bool connect(CamFinder& finder, cv::VideoCapture& cap)
{
	finder.scanNetwork();
	std::string camera_url = finder.IPstring;
	cap.open(camera_url);
	return cap.isOpened();
}

int main(int argc, char* argv[])
{
	signal(SIGINT, signal_handler);

	std::cout << "Crowdsight Sensor v" << VERSION << std::endl;

	// Variables used to calculate frames per second:
	DWORD dwFrames;
	DWORD dwCurrentTime;
	DWORD dwLastUpdateTime;
	DWORD dwElapsedTime;
	DWORD dwLastFrameRate;
	TCHAR szFPS[32];

	// Zero out the frames per second variables:
	dwFrames = 0L;
	dwCurrentTime = 0L;
	dwLastUpdateTime = 0L;
	dwLastFrameRate = 0L;
	dwElapsedTime = 0L;
	szFPS[0] = '\0';

	// Video Capture object
	cv::VideoCapture cap;

	DataProcessor handler;
	CamFinder finder;
	// Gather configuration from the config file
	std::cout << "Configuring..." << std::endl;
	parseIniFile("config.ini");

	// Settings
	std::string auth_key = getOptionToString("auth_key");
	std::string crowdsightData = getOptionToString("crowdsight_data");
	bool flipFrame = getOptionToInt("flip_frame") == 0 ? false : true;
	std::string camera_id = getOptionToString("camera_id");
	std::string target_IP = getOptionToString("target_ip");
	finder.StringToIP(target_IP);
	std::string camera_mac = getOptionToString("camera_mac");
	finder.StringToMAC(camera_mac);
	finder.scanNetwork();
	std::string camera_url = finder.IPstring;
	int v_width = getOptionToInt("v_width");
	int v_height = getOptionToInt("v_height");
	int cam_fps = getOptionToInt("cam_fps");
	int scaleNum = getOptionToInt("scaleNum");
	int scaleDem = getOptionToInt("scaleDen");
	float scale = scaleNum/(float)scaleDem;
	int rotation = getOptionToInt("rotation");

	WINDOWSIZE_X = v_width * scale;
	WINDOWSIZE_Y = v_height * scale;
	
	int mar_top = getOptionToInt("mar_top");
	int mar_bot = getOptionToInt("mar_bot");
	int mar_left = getOptionToInt("mar_left");
	int mar_right = getOptionToInt("mar_right");

	CONFIG_MODE = getOptionToInt("CONFIG_MODE");

	IMPRESSION_THRESHOLD_A = getOptionToInt("IMPRESSION_THRESHOLD_A");
	IMPRESSION_THRESHOLD_B = getOptionToInt("IMPRESSION_THRESHOLD_B");
	SENT_THRESHOLD = getOptionToInt("SENT_THRESHOLD");
	DORMANT_THRESHOLD = getOptionToInt("DORMANT_THRESHOLD");
	FREQUENCY_THRESHOLD = getOptionToInt("FREQUENCY_THRESHOLD");
	SUBDIV_X = getOptionToInt("SUBDIV_X");
	SUBDIV_Y = getOptionToInt("SUBDIV_Y");

	std::string collector_host = getOptionToString("collector_host");
	int collector_port = getOptionToInt("collector_port");
	bool debug_log = getOptionToInt("debug_log") == 0 ? false : true;
	cleanupIniReader();

	// Make sure we have a camera_id
	if(camera_id.empty()) {
		// TODO: copy the value, not the pointer
		camera_id = camera_url;
	}

	
	
	// Open video source
	std::cout << "Opening Camera..." << std::endl;
	// TODO: parameters (connect to IP camera)
	//cap.open(0);
	//cap.open("http://66.184.211.231/mjpg/video.mjpg");
	camera_url = finder.IPstring;
	connect(finder, cap);
	
	

	// Create crowdsight instance
	std::cout << "Initializing Crowdsight..." << std::endl;
	CrowdSight crowdsight(crowdsightData);
	
	// Authenticate crowdsight instance
	std::cout << "Authenticating Crowdsight..." << std::endl;
	while (!crowdsight.authenticate(auth_key))
	{
		std::cerr << crowdsight.getErrorDescription() << std::endl;
		//return 11;
	}

	// Setup the UDP client
	WSAData wsaData;
	int nCode;
	if((nCode = WSAStartup(MAKEWORD(1,1), &wsaData)) != 0) {
		std::cerr << "Unable to startup winsock, error code " << nCode << "." << std::endl;
		return 110;
	}
	int socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(socketHandle <= 0) {
		std::cerr << "Unable to create socket." << std::endl;
		return 111;
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = 0;

	if(bind(socketHandle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
		std::cerr << "Unable to bind socket." << std::endl;
		return 112;
	}

	// Setup video capture resolution 
	// TODO: parameters
	cap.set(CV_CAP_PROP_FRAME_WIDTH, (int)(v_width*scale));
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, (int)(v_height*scale));
	cap.set(CV_CAP_PROP_FPS, cam_fps);
	cv::Mat rotMat;
	if(rotation!=0)
	{
		 rotMat = cv::getRotationMatrix2D(cv::Point2f(v_width*0.5, v_height*0.5), rotation, 1);
		 rotMat *=scale;
	}
	cv::Mat frame;

	// Define frameskip if necessary.
	int processEveryNthFrame = 1;
	// Keep track of how many frames are processed
	unsigned long frameCount = 0L;

	// Grab one frame
	//cap >> frame;
	if(!cap.read(frame)) {
		connect(finder, cap);
		std::cerr 
			<< "OpenCV was unable to read a new frame." 
			<< std::endl;
		return 12;
	}
	
	// Define region of interest here if required
	int marginTop     = mar_top;
	int marginBottom  = mar_bot;
	int marginLeft    = mar_left;
	int marginRight   = mar_right;
	cv::Rect roi;

	// Configure crowdsight
	// TODO: shouldn't this just be picked up by the settings.ini file?
	crowdsight.useAge( true );
	crowdsight.useGender( true );
	crowdsight.useMood( true );
	crowdsight.useHeadPose( true );
	crowdsight.useScaleFactor( true );

	int maxNumPeople = 15;

	// use lists and map to smooth results
	std::list<float> happyList, surpriseList, angerList, disgustedList, fearList, sadnessList;
	std::map<std::string, std::list<cv::Rect> > personPositions;
	  cv::namedWindow( HUMAN_NAME );
	//Start main processing loop
	std::cout << "Starting main loop." << std::endl;
	std::vector<Person> people;

	//cv::Mat src = cv::imread( "../floor.png", 1 );

  cv::Size s(3,3);
	
	while( running )
	{
		crowdsight.setMaxNumPeople( maxNumPeople );
		frameCount++;
		

		// Grab next frame
		if(!cap.read(frame)) {
			std::cerr
				<< "OpenCV was unable to read a new frame."
				<< std::endl;
				connect(finder, cap);
			// TODO: handle this error condition
			continue;
		}

		// If frame is empty break
		if ( frame.empty() )
		{
			// TODO: this is an error condition that needs addressed
			std::cerr << "empty frame" << std::endl;
			continue;
		}
		else
		{
			cv::Mat tmp;
			//std::cout<<scale;
			if(rotation!=0)
			{
				cv::warpAffine(frame, tmp, rotMat, frame.size(), cv::INTER_CUBIC);
				frame = tmp.clone();
				//resize(tmp,frame,cv::Size(),scale,scale,cv::INTER_CUBIC);
			}
			else
			{
				resize(frame,tmp,cv::Size(),scale,scale,cv::INTER_CUBIC);
				frame = tmp.clone();
			}
			
		}
		// Flip frame horizontally if capturing from webcam
		if(flipFrame) {
			cv::flip(frame, frame, 1);
		}

		// Specify the roi that will be extracted from the frame
		roi.x      = marginLeft;                    // pixels to skip from the left
		roi.y      = marginTop;                     // pixels to skip from the top
		roi.width  = frame.cols-marginLeft-marginRight;  // width of roi
		roi.height = frame.rows-marginTop-marginBottom; // height of roi


		
		// Proces current frame.
		// Process function evaluates the frames contents and
		// must be called before getCurrentPeople();
		if ( frameCount % processEveryNthFrame == 0 && frameCount > 0 ) 
		{
			handler.updateClusters();
			if ( !crowdsight.process( frame ) ) 
			{
				std::cerr << crowdsight.getErrorDescription() << std::endl;
				continue;
			}
		}

	

    //Press Esc, q to quit the program
    char key = cv::waitKey(10);

		// Get the list of people in the last processed frame
		if (!crowdsight.getCurrentPeople(people))
		{
			std::cerr << crowdsight.getErrorDescription() << std::endl;
			continue;
		}

		// Calculate the number of frames per one second:
		dwFrames++;
		dwCurrentTime = GetTickCount(); // Even better to use timeGetTime()
		dwElapsedTime = dwCurrentTime - dwLastUpdateTime;

		// Debugging
		if(debug_log) {
			for ( unsigned int i = 0; i < people.size(); ++i )
				{
					// Get person at current index
					Person person = people.at(i);

					std::cout
						<< ((dwElapsedTime >= 1000) ? "UDP," : "   ,")
						<< std::time(0) << ","
						<< dwLastFrameRate << ","
						<< person.getID() << ","
						<< person.getAge() << ","
						<< person.getGender() << ","
						<< person.getMood() << ","
						<< person.getAttentionSpan()
						<< std::endl;
			}
		}
 
		// Only process ~ once per second
		//if(dwElapsedTime >= 1000)
		//{
			dwLastFrameRate = (dwFrames * 1000.0 / dwElapsedTime);
			dwFrames = 0;
			dwLastUpdateTime = dwCurrentTime;

			JsonBox::Array jPeople;

			for ( unsigned int i = 0; i < people.size(); ++i )
			{
				//std::vector< std::vector<int> > clothingColors;
				//std::vector< float > emotionPredictions;

				// Get person at current index
				Person person = people.at(i);

				/*********************************** RETRIEVE PERSON INFO ***********************************/

				cv::Rect    face              = person.getFaceRect();      // Retrieve the person's face
				cv::Point   right_eye         = person.getRightEye();      // Retrieve left and right eye locations of the person. Eye location is relative to the face rectangle.
				cv::Point   left_eye          = person.getLeftEye();
				std::string id                = person.getID();
				float       ageValue          = person.getAge();
				float       genderValue       = person.getGender();
				float       moodValue         = person.getMood();
				int64_t     attentionSpan     = person.getAttentionSpan(); // Get person's attention span. This value is returned in milliseconds
				//bool        hasClothingColors = person.getClothingColors( clothingColors );
				bool        returningCustomer = person.isReturningCustomer();
				bool        isFromDatabase    = person.isFromDatabase();

				//person.getEmotions( emotionPredictions );

				// Determine male or female
				std::string gender = "male";
				if(genderValue >= 0.0) {
					gender = "female";
				}

				Impression tempImp;
				tempImp.age = ageValue;
				tempImp.gender = genderValue;
				tempImp.pos.x = face.x;
				tempImp.pos.y = face.y;
				tempImp.countdown = 50;

				int intID = atoi(id.c_str());
				if(handler.checkNewID(intID))
				{
					if(CONFIG_MODE==1)
						std::cout<<"NEW CLUSTER of ID:"<<intID<<" at "<<face.x<<","<<face.y<<"\n";
					handler.addCluster(intID, tempImp);
				}
				else
				{
					handler.crowd[intID].findIndex(tempImp,intID);
					//handler.crowd[intID].addImpression(tempImp, handler.IMPRESSION_THRESHOLD_A, handler.IMPRESSION_THRESHOLD_B, handler.SENT_THRESHOLD, intID);
				}

				cv::putText(frame,id, cv::Point(face.x, face.y), cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(255,0,255),2,8);
				cv::circle(frame,cv::Point(face.x+face.width/2,face.y+face.height/2),face.width+5/2, cv::Scalar(255,255,255), 5);
			}
			if((!frame.empty())&&(CONFIG_MODE==1))
			{
				cv::imshow( HUMAN_NAME, frame);
			}

			

			for(int i=0;i<handler.listToSend.size();++i)
			{
				if(handler.listToSend[i].sentCount<SENT_THRESHOLD)
				{
					if(CONFIG_MODE==1)
						std::cout<<"SEND:"<<"\nID:"<<handler.listToSend[i].clusterID<<"/"<<handler.listToSend[i].index
							 <<"\nAGE:"<<handler.listToSend[i].aveImpression.age
							 <<"\nGENDER:"<<handler.listToSend[i].gender_s<<"\n";
					++handler.listToSend[i].sentCount;
					// Person Json object
					JsonBox::Object jPerson;
					jPerson["id"] = JsonBox::Value(handler.listToSend[i].clusterID);
					jPerson["age"] = JsonBox::Value(handler.listToSend[i].aveImpression.age);
					jPerson["gender"] = JsonBox::Value(handler.listToSend[i].gender_s);
					//jPerson["mood"] = JsonBox::Value(moodValue);
					//jPerson["attention"] = JsonBox::Value(static_cast<int>(attentionSpan));
					jPeople.push_back(jPerson);
					// JSON
				}
			}

			handler.listToSend.clear();
			JsonBox::Object o;
			o["camera_id"] = JsonBox::Value(camera_id);
			o["status"] = JsonBox::Value("online");
			o["msgtype"] = JsonBox::Value("demographics");
			o["timestamp"] = JsonBox::Value((int)std::time(0));
			o["fps"] = JsonBox::Value(static_cast<int>(dwLastFrameRate));
			o["people"] = JsonBox::Value(jPeople);
			
			if(people.size() > 0) {
				std::stringstream ss;
				JsonBox::Value(o).writeToStream(ss, false, false);
				ss << std::endl;
				
				std::string jsonString = ss.str();
				const char *json = jsonString.c_str();
				
				//std::cout << json;

				sockaddr_in destAddress;
				destAddress.sin_family = AF_INET;
				destAddress.sin_addr.s_addr = inet_addr(collector_host.c_str());
				destAddress.sin_port = htons( (u_short) collector_port );

				int sentBytes = sendto( socketHandle, json, strlen(json), 0, (sockaddr*)&destAddress, sizeof(sockaddr_in) );

			}
		//}

		// Clear the people array
		people.clear();
	}
	closesocket(socketHandle);
	WSACleanup();

	return 0;
}


