/*! \mainpage CrowdSight SDK
*
* \section intro_sec Introduction
*
* CrowdSight SDK is a cross platform software library for automated semantic analysis of people in video and images.
* CrowdSight SDK can be used to automatically analyze faces in real time via a simple webcam, and communicate the
* resulting information to the third party application.
* The SDK allows for retrieving tailored information depending on the specific requirements of the companies or
* individuals.
* Currently, CrowdSight SDK can estimate the eye locations, head pose (yaw and pitch), mood, age, gender, clothing colors and attention span of the subjects.
* In addition to this, CrowdSight SDK can be used for face tracking and recognition under a wide range of imaging conditions. Face models can be
* stored, (un)loaded and renamed before system startup as well as during runtime.
* The SDK is based on state-of-the-art technologies developed in collaboration with the University of Amsterdam.
*
* The SDK can be easily integrated in third party software as a collection of
* C++ libraries for Windows, Mac and Linux platforms.
*
* \section usage Usage
*
* CrowdSight SDK operates on frame sequences. Each frame is processed and the information about the detected subjects can be retrieved through getter functions.
* In the pseudo-code below, a simple example on how to retrieve and display information for each subject is shown:
*
* \code
*
*    //Initialize CrowdSight and the capturing device/video input
*    CrowdSight crowdsight(data_dir);
*    crowdsight.authenticate(licence_key); // authentication with our server is necessary
*
*    for (;;)
*    {
*        capture >> frame; // get a new frame from the video stream
*        crowdsight.process(frame);
*        crowdsight.getCurrentPeople(people);// extract current people in the frame
*        for (int i=0; i<people.size();i++)
*        {
*            cv::Rect face = (people.at(i)).getFaceRect();
*            draw(face)
*            idString << "ID #" << people.at(i).getID();
*            genderString << "Gender: " << ((people.at(i).getGender())<0?"male":"female");
*            ageString << "Age: " << people.at(i).getAge();
*            moodString << "Mood" << people.at(i).getMood();
*            drawText(frame, face, idString);
*            drawText(frame, face, genderString);
*            drawText(frame, face, ageString);
*            drawText(frame, face, moodString);
*       }
*       counterString<<" People Counter: "<< crowdsight.getPeopleCount();
*       drawText(frame, face, counterString);
*    }
*
* \endcode
*
* For the full code, refer to the example implementation provided with the SDK. Additional usage examples can be provided upon request.
*
* \section scenario Scenario
* CrowdSight SDK is designed for a multi-person scenario. It is recommended that the face detections are at least 50 x 50 pixels in order to be processed.
* Camera zoom may be used to adhere to this requirement.
*
* \section settings Settings
* CrowdSight SDK offers flexibility by granting access to some internal parameter settings through the settings.ini file.
* This file will be generated in the directory where the executable is launched that is
* calling the CrowdSight SDK:
*
* \code
*   [ClothingStyle]
*   ClothingHistogramBins = 15
*   NrOfClothingColors = 3
*
*   [Detectors]
*   UseAge = 1
*   UseGender = 1
*   UseHeadPose = 1
*   UseMood = 1
*
*   [FaceDetector]
*   AdaptiveFilterUseFaces = 0
*   FaceConfidence = 0.0
*   LoadAdaptiveFilter = 0
*   MaxFaceSize = 0
*   MaxNumPeople = 15
*   MinFaceSize = 50
*   NumSamplesAdaptiveFilter = 100
*   ScaleFactor = 0
*   UseAdaptiveDetector = 0
*   UseAdaptiveFiltering = 1
*
*   [FaceMatching]
*   GraphSize = 500
*   MatchingLevel = 0.35
*
*   [FaceRecognition]
*   BootstrapGain = 0.5
*   BufferSize = 15
*   ConstrainedRecognitionLevel = 0.5
*   LoadModels = 0
*   ObservationGain = 0.5
*   SaveModels = 0
*   UseFaceRecognition = 0
*   VisualRecognitionLevel = 0.8
*
*   [ROI]
*   MarginBottom = 0
*   MarginLeft = 0
*   MarginRight = 0
*   MarginTop = 0
*   UseROI = 0
*
*   [ReturningCustomer]
*   TimeSpan = 10
*
* \endcode
*
* The settings are discussed in the following. NOTE: none of the parameters is allowed to be a negative value
*
* [ClothingStyle]
* The developer is offered control over the size of the internal RGB histogram. Increasing the number of bins will result in more
* detailed color retrieval at the expense of computational costs.
* The desired number of colors to be retrieved is set to 3 by default and can be increased or decreased.
*
* [Detectors]
* The developer decides which detectors are employed for analysis.
* The *age*, *gender*, *mood* and *headpose* detectors are (de)activated by setting the corresponding parameter to 1 (0).
* The age and (to a lesser extent) gender and mood detectors are sensitive to headpose. The developer is advised to restrict
* measurements of age, gender and mood to be made only for limited headpose ranges.
*
* [FaceDetector]
* The developer is offered some control over the face detector. This control is mainly needed to deal with false detections,
* because face detection can be heavily influenced by cluttered environments or illumination changes.
* The following parameters can be set for performance optimization:
*
* - \e UseAdaptiveDetector: if set to 1 (true), it enables the adaptive face detector. For details, read the manual at data/ob/manual.txt.
* - \e UseAdaptiveFiltering: the adaptive filter learns an online background model in order to decrease the number of false detections.
* If this setting is activated, it is advised to have no people in front of the camera on startup when the background model is learned.
* The model is stored in the data directory.
* - \e AdaptiveFilterUseFaces: if set to 1 (true), the adaptive filter will consider true positive detections during training of the adaptive filter.
* - \e NumSamplesAdaptiveFilter: defines the number of true negative samples extracted by the adaptive filter.
* - \e LoadAdaptiveFilter: if a background model is stored in the data directory (see *UseAdaptiveFiltering*),
* it can be loaded on startup. No new background model is learned if this setting is activated.
* - \e FaceConfidence: defines a level of confidence for passing observations as actual faces and ranges over [0,1].
* Increasing this threshold will decrease the number of false detections. There exists a general tradeoff between
* the number of false detections (false positives) and the number of missed detections (false negatives).
* - \e MaxFaceSize: defines the maximum size of a face in pixels for further analysis.
* - \e MinFaceSize: defines the minimum size of a face in pixels for further analysis. We suggest not to use faces smaller than 50x50 pixels
* since a low resolution will affect the accuracy of all subsequent processing steps.
* - \e MaxNumPeople: defines the maximum number of people that can be processed simultaniously.
* - \e ScaleFactor: by switching on the scale factor (1/0), the face detector
* will detect faces on a smaller number of scales in the image. This results in a great speedup at the expense of some accuracy loss.
*
* [FaceMatching]
* is a light-weight algorithm for the task of recognizing and tracking people throughout a sequence of frames.
*
* - \e GraphSize: the maximum duration (in number of frames) within which a person may be recognized. The default is set to 500. Depending on
* system specifications, this can be increased significantly.
* - \e MatchingLevel: a threshold that expresses the minimum required confidence for a match between a new observation and an existing model of a person.
* The parameter ranges over [0,1]; it is set to 0.35 by default. Increasing the parameter value leads to stricter matching.
*
* [FaceRecognition]
* can be used to identify people that re-appear in the field of view.
* Also, this module is complementary to (i.e. greatly improves over) the face matching functionality by employing online -learning and -classification schemes.
* Additionally, the face models that are learned may be stored and loaded for future re-identification.
*
* - \e BootstrapGain: controls the number of examples that are generated from a single face observation for bootstrapping a visual face model.
* The parameter ranges over [0,1], corresponding to [0,100] learning examples. More examples generally lead to more accurate bootstrapping,
* but also increases the computational costs of the algorithm.
* - \e BufferSize: limits the amount of face models that may be learned online. Larger buffers imply higher computational costs.
* - \e ConstrainedRecognitionLevel: controls the amount of distance and duration that is allowed between two observations within which
* they are always considered to be the same person. Internally, spatial constraints are thus enforced to rapidly decrease their influence
* as time elapses. The ConstrainedRecognitionLevel offers control over the pace at which the influence is decreased, and ranges over [0,1]
* (larger values imply stricter recognition).
* - \e LoadModels: activating this setting causes all face models that reside on disk to be loaded from the data/db/ folder on startup (see *SaveModels*).
* Based on these models, the algorithm will attempt to re-identify people that are detected in the sequence.
* - \e ObservationGain: controls the number of examples that are acquired online for learning face models. Thus, as a person is tracked
* throughout the sequence, more and more examples may be collected, which generally improves the model accuracy for re-identification
* (as well as further tracking). The parameter is interpreted in the same way as the *BootstrapGain*.
* - \e SaveModels: activating this setting causes all face models to be saved in the data/db/ folder.
* - \e UseFaceRecognition: (de)activates the module.
* - \e VisualRecognitionLevel: threshold in [0,1] for determining the minimum confidence of a face model for classifying a new observation.
* This level should be fairly high (0.8 by default) in order to prevent false recognition.
*
* [ROI]
* A Region Of Interest can be specified in terms of offsets from the top, bottom, left and right of the frame. CrowdSight analysis will be applied only to
* this specified region. As a result, the computational needs are lowered.
*
* - \e UseROI: (de)activates ROI usage.
* - \e MarginBottom: offset in pixels from bottom.
* - \e MarginLeft: offset in pixels from left.
* - \e MarginRight: offset in pixels from right.
* - \e MarginTop: offset in pixels from the top.
*
* [ReturningCustomer]
* The timespan in seconds during which a person can reside outside the field of view before being recognized as a returning customer.
*
* \section requirements Minimum requirements
* - Intel Core 2 Duo 1.6GHZ or better.
* - 2GB RAM
* - Input frames with a resolution of at least 640 x 480
* - Active Internet connection
*/

#ifndef CROWDSIGHT_H
#define CROWDSIGHT_H

#include <opencv/cv.h>
#include <vector>
#include <string>
#include <stdint.h>

#ifdef ANDROID
#include <jni.h>
#endif

/** @cond */
// Helper definitions for shared library support
// http://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
  #define CROWDSIGHT_DLL_IMPORT __declspec(dllimport)
  #define CROWDSIGHT_DLL_EXPORT __declspec(dllexport)
  #define CROWDSIGHT_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define CROWDSIGHT_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define CROWDSIGHT_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define CROWDSIGHT_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define CROWDSIGHT_DLL_IMPORT
    #define CROWDSIGHT_DLL_EXPORT
    #define CROWDSIGHT_DLL_LOCAL
  #endif
#endif

#ifdef CROWDSIGHT_DLL               // defined if CrowdSight is compiled as a shared library
    #ifdef CROWDSIGHT_EXPORTS       // defined if compiling CrowdSight
      #define CROWDSIGHT_API CROWDSIGHT_DLL_EXPORT
    #else
      #define CROWDSIGHT_API CROWDSIGHT_DLL_IMPORT
    #endif
    #define CROWDSIGHT_LOCAL CROWDSIGHT_DLL_LOCAL
#else                               // CrowdSIght is a static library
    #define CROWDSIGHT_API
    #define CROWDSIGHT_LOCAL
#endif


// Deprecation macros
#ifdef __GNUC__
  #define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
  #define DEPRECATED(func) __declspec(deprecated) func
#else
  #pragma message("WARNING: You need to implement DEPRECATED for this compiler")
  #define DEPRECATED(func) func
#endif
/** @endcond */

/*! \brief OperationMode enumerates all possible CrowdSight operation modes 
 */
enum OperationMode {
  DEVELOPER,          /*!< Default mode. Used for development on a new or previously registered developer machine.*/
  REDISTRIBUTION,     /*!< Redistribution mode. Used to produce the final binary to be distributed to your clients.*/
  SERVER              /*!< Server mode. Used on a server machine in combination with a CrowdSight server license.*/
};

/*! \brief ColorSpace enumerates all supported color spaces for conversion of raw image data to an OpenCV BGR format
 */
enum ColorSpace {
  RGB = 0, /*!< data is in RGB format */
  BGR,     /*!< data is in BGR format */
  YUV,     /*!< data is in YUV format */
  YUV420   /*!< data is in YUV420 format, mainly used for Android cameras */
};

/*! \brief FeaturesRequest defines which features will be extracted for each Person on getCurrentPeople.
 *  To be used along with server operation mode to define the credits to be used. On developer or redistribution
 *  operation mode getCurrentPeople should use the default parameter (ALL_FEATURES)
 */
struct CROWDSIGHT_API FeaturesRequest {
  bool age;
  bool gender;
  bool mood;
  bool head_gaze;
  bool eye_location;
  bool attention_span;
  bool clothing_colors;
  bool emotions;
  bool returning_customer;
};

extern const CROWDSIGHT_API FeaturesRequest ALL_FEATURES; /*!< Request all features */

class PersonImpl;
class CrowdSightImpl;

class CROWDSIGHT_API Person
{
public:

  /*! \brief Person structure returned by CrowdSight SDK
   *
   * */
  Person( PersonImpl & p );
  Person( const Person & p );
  Person & operator=( const Person & p );


  /*! \brief Person structure destructor
   *
   * */
  ~Person();

  /*! \brief Returns the person Identifier (label). Each person has a unique ID.
   * Note: this number does not represent the number of people.
   *
   * @return the unique ID of the person
   * */
  const std::string & getID();

  /*! \brief Returns the estimated age for the person. This number should be an indication
   * of the age category, rather than an accurate estimation of age. The function returns
   * 0 if the age estimation is not available yet.
   *
   * @return the estimated age in years.
   * */
  int getAge();

  /*! \brief Returns the estimated gender for the person
   *
   * @return A value between -1 (Male) and +1 (Female). Values around 0 should be
   * considered as "uncertain".
   * */
  float getGender();

  /*! \brief Returns a rectangle defining the x, y, width and height of the face
   * of the person in image coordinates.
   *
   * @return cv::Rect with the face coordinates
   * */
  cv::Rect  getFaceRect();

  /*! \brief Returns the estimated mood of the person.
   *
   * @return values in the range [-1,1]. Positive values correspond to positive mood
   * (eg: happy), negative values correspond to negative mood (eg: sad), near zero values
   * mean neutral mood
   * */
  float getMood();

  /*! \brief Returns the estimated head yaw of the person.
   *
   * @return Yaw value between -1 and 1, where -1 is -40 degrees, and +1 is +40 degrees.
   * */
  float getHeadYaw();

  /*! \brief Returns the estimated head pitch of the person.
   *
   * @return Pitch value between -1 and 1, where -1  is -30 degrees, and +1 is +30 degrees.
   * */
  float getHeadPitch();

  /*! \brief Returns the position in pixels of right eye of the person, relative to the
   * face rectangle (returned by getFaceFect()).
   *
   * @return cv::Point with the right eye coordinates
   * */
  cv::Point getRightEye();

  /*! \brief Returns the position in pixels of left eye of the person, relative to the
   * face rectangle (returned by getFaceFect()).
   *
   * @return cv::Point with the left eye coordinates
   * */
  cv::Point getLeftEye();

  /*! \brief Returns the timestamp of the current person as elapsed time since the
   * CrowdSight initialization.
   *
   * @return value in milliseconds
   * */
  int64_t getTime();

  /*! \brief Returns the estimated attention time of a person.
   *
   * The estimated attention time represents the time in which the person is actively looking
   * towards the camera. If the person is not observed for more than 3 seconds, the attention span will reset to 0.
   *
   * @return value in milliseconds
   * */
  int64_t getAttentionSpan();

  /*! \brief Checks if a person is returning since a period of time
   *
   * @return true if a person is returning since a period of time
   */
  bool isReturningCustomer();

  /*! \brief Retrieve a set of dominant clothing colors in RGB format. Returns false if the cloth patch
   * is not within the image plane
   *
   * @deprecated This function is deprecated. Use getClothingColors( std::vector< std::vector<int> > & clothingColorsOut );
   * instead.
   *
   * @param[in] numColors number of dominant colors to retrieve
   * @param[out] clothingColorsOut a vector of the top colors in RGB values (0-255)
   * @return false if cloth patch is not within the image plane
   */
  DEPRECATED( bool getClothingColors( int numColors, std::vector< std::vector<int> > & clothingColorsOut ) );

  /*! \brief Retrieve a set of dominant clothing colors in RGB format.
   * Returns false if the cloth patch is not within the image plane. The number of colors to be retrieved
   * can be set in settings.ini.
   *
   * @param[out] clothingColorsOut a vector of the top colors in RGB values (0-255)
   * @return false if cloth patch is not within the image plane
   */
  bool getClothingColors( std::vector< std::vector<int> > & clothingColorsOut );

  /*! \brief Checks if a person is recognized from database models. NOTE: This function can
   * only be used when Face Recognition is in use.
   *
   * @return true if person is recognized from a database model
   */
  bool isFromDatabase();

  /*! \brief Returns the current emotion intensities
   *
   * Return a vector of floats in [0,1] which represent the current emotion intensities:
   * - vector[0]: happy
   * - vector[1]: surprised
   * - vector[2]: angry
   * - vector[3]: disgusted
   * - vector[4]: afraid
   * - vector[5]: sad
   *
   * @remarks Works on frontal faces with a minimum face size of 150 by 150 pixels
   * @param[out] emotionsOut    The vector with emotion intensities
   * @return Success or failure
   */
  bool getEmotions( std::vector<float> & emotionsOut );


private:
  PersonImpl * mPerson;
};

class CROWDSIGHT_API CrowdSight
{
public:
  /*! \brief Instantiate the SDK.
   *
   * @param[in] dataDir    Path to the CrowdSight data directory containing the SDK resource files
   * @param[in] opMode     OperationMode of current CrowdSight instance. Default, DEVELOPER
   */
#ifdef ANDROID
  CrowdSight( jobject context, const std::string &mDataDir = "./data/", const OperationMode opMode = DEVELOPER );
#else
  CrowdSight( const std::string &mDataDir = "./data/", const OperationMode opMode = DEVELOPER );
#endif // ANDROID

 /*! \brief Called by delete to release the SDK */
  ~CrowdSight();

  /*! \brief Returns the current version of the SDK
   *
   * This functions returns the CrowdSight version as a string. The string
   * is in the form of xx.yy where 'xx' are the two digits used for the
   * major version number and 'yy' are the two digits used for the minor
   * version number.
   *
   * @return string The current version number of the SDK
   */

  static std::string getVersion();

  /*! \brief Authenticate user to secure server
   *
   * This function will connect to the secure server and obtain the
   * credentials to use the SDK. It has the optional ability to connect
   * through a proxy server. In order to use the getter functions (such
   * as getPeopleCount()), this function must be called first.
   *
   * @param[in] key    	        The license key provided by sightcorp
   * @param[in] proxyAddress    Optional proxy address
   * @param[in] proxyPort    	Optional proxy port
   * @param[in] proxyUserName   Optional user name
   * @param[in] proxyPassword   Optional password
   *
   * @return success or failure of the authentication step. In case of failure, the error message
   * can be retrieved using the getErrorDescription() function.
   */
  bool authenticate( const std::string & key,
                     const std::string & proxyAddress = "",
                     const int           proxyPort = 0,
                     const std::string & proxyUserName = "",
                     const std::string & proxyPassword = "" );

  /*! \brief Returns the amount of remaining credits for the current license
   *
   * This function can be called after the user has authenticated with the server.
   * In DEVELOPER OperationMode it returns the amount of seconds remaining until
   * the license expiration. In SERVER OperationMode it returns the amount of usages
   * remaining. In REDISTRIBUTION OperationMode or in case of error it returns -1,
   * indicating that the remaining credits information is not available.
   *
   * @remarks Does not use a credit
   * @pre CrowdSight instance must be successfully authenticated using the CrowdSight::authenticate function
   * @return The remaining amount of credits for this license or -1
   */
  int getRemainingCredits();

  /*! \brief Process a new frame
   *
   * Call this function to process a new frame in the video sequence or a single image. Information
   * is extracted from the frame, and can then be retrieved using the getter functions. Before calling this
   * function, the SDK must successfuly authenticate against the secure server through the
   * authenticate() function.
   *
   * @param[in] image    The current video frame. Needs to be a three channel cv::Mat image.
   * @param[in] region   The region of interest as defined by the user. This is useful to select
   * the area of the image in which the people need to be detected.
   * @param[in] singleImage   Set to false to process a video, set to true to process separate images
   *
   * @return true if the function is successfully completed. In case of failure, the error message
   * can be retrieved using the getErrorDescription() function.
   * */
  bool process( const cv::Mat & image, const cv::Rect & roi, bool singleImage = false );

  /*! \brief Process a new frame
   *
   * Call this function to process a new frame in the video sequence or a single image. Information
   * is extracted from the frame, and can then be retrieved using the getter functions. Before calling this
   * function, the SDK must successfuly authenticate against the secure server through the
   * authenticate() function.
   *
   * @param[in] image    The current video frame. Needs to be a three channel cv::Mat image.
   * @param[in] singleImage   Set to false to process a video, set to true to process separate images
   *
   * @return true if the function is successfully completed. In case of failure, the error message
   * can be retrieved using the getErrorDescription() function.
   * */
  bool process( const cv::Mat & image, bool singleImage = false );

  /*! \brief Process a new frame
   *
   * Call this function to process a new frame in the video sequence or a single image. Information
   * is extracted from the frame, and can then be retrieved using the getter functions. Before calling this
   * function, the SDK must successfuly authenticate against the secure server through the
   * authenticate() function.
   *
   * @param[in] image   The current video frame. Needs to be a three channel cv::Mat image.
   * @param[in] faces   A vector with rectangles defining the location (x, y) and (width, height) of faces
   * in image. Use this function when the use of 3rd-party face detection libraries is preferred.
   * @param[in] singleImage   Set to false to process a video, set to true to process separate images
   *
   * @return true if the function is successfully completed. In case of failure, the error message
   * can be retrieved using the getErrorDescription() function.
   * */
  bool process( const cv::Mat & image, const cv::vector<cv::Rect> & faces, bool singleImage = false );

  /*! \brief Get the list of people in the current frame
   *
   * Call this function to retrieve the vector of people in the current frame. The process() function
   * needs to be called before this. Each of the elements in the vector can then be queried with
   * getters (see Person class methods)
   *
   * @param[out] people The detected people in the current frame are returned as a vector of
   * @param[in] features The features that will be extracted for each detected person. On developer
   * and redistribution mode the default parameter ALL_FEATURES might be used as it won't use credits
   * person structures
   *
   * @return true if the function is successfully executed
   * */
  bool getCurrentPeople( std::vector<Person> & people, const FeaturesRequest & features = ALL_FEATURES );

  /*! \brief Get the current number of unique people observed since startup.
   *
   * Call this function to retrieve the number of people counted since the application was started.
   * If a person comes back after a certain amount of time, he/she will be counted as a new person.
   * Note: the counter is automatically corrected if a person is initially double counted and
   * recognized later on as double counted by the system
   *
   * @return the estimated people count
   * */
  int getPeopleCount();

  /*! \brief Get maximum number of people allowed to be detected during a process call.
   *
   * @return the estimated people count
   * */
  int getMaxNumPeople();

  /*! \brief Save the model of a given ID.
   *
   * The model will be stored in 2 files in the data/db folder of CrowdSight. NOTE: This function can
   * only be used when Face Recognition is in use.
   *
   * @return true if a model can be saved
   * */
  bool saveModel( std::string & id );

  /*! \brief Load the model of a given ID.
   *
   * The model is loaded from 2 files within the data/db folder of CrowdSight. NOTE: This function can
   * only be used when Face Recognition is in use.
   *
   * @return true if a model can be loaded
   * */
  bool loadModel( std::string & id );

  /*! \brief Unload the model of a given ID.
   *
   * The model is unloaded from memory. NOTE: This function can
   * only be used when Face Recognition is in use.
   *
   * @return false if the model is not in memory
   * */
  bool unloadModel( std::string & id );

  /*! \brief Rename the model from the given id to the newId.
   *
   * The model is renamed in memory. NOTE: This function can
   * only be used when Face Recognition is in use.
   *
   * @return false if the model is not in memory
   * */
  bool renameModel( std::string & id, std::string & newId );

  /*! \brief Change the matching level for the matching algorithm
   *
   * @param[in] matchingLevel
   */
  void setMatchingLevel( float matchingLevel );

  /*! \brief Change the face confidence for the face detector
   *
   * @param[in] faceConfidence
   */
  void setFaceConfidence( float faceConfidence );

  /*! \brief Set the minimum allowed size for a face detection
   *
   * @param[in] minFaceSize
   */
  void setMinFaceSize( int minFaceSize );

  /*! \brief Set the maximum allowed size for a face detection
   *
   * @param[in] maxFaceSize
   */
  void setMaxFaceSize( int maxFaceSize );

  /*! \brief Set the usage of the scale factor, speeding up face detection but losing accuracy
   *
   * @param[in] use
   */
  void useScaleFactor( bool use );

  /*! \brief Set the maximum number of people that are allowed to be detected during a process call.
   * The order in which people are detected is from nearby to faraway.
   *
   * @param[in] maxNumPeople the maximum number of people to be detected
   */
  void setMaxNumPeople( int maxNumPeople );

  /*! \brief Load/unload the Age classifier
   *
   * @param[in] use
   */
  void useAge( bool use );

  /*! \brief Load/unload the Gender classifier
   *
   * @param[in] use
   */
  void useGender( bool use );

  /*! \brief Load/unload the Mood classifier
   *
   * @param[in] use
   */
  void useMood( bool use );

  /*! \brief Turn on/off the learning of models.
   * When turned off, only the models currently loaded or learned will  be used for recognition.
   * NOTE: This function can only be used when Face Recognition is in use.
   *
   * @param[in] use
   */
  void useLoadedModelsOnly( bool use );

  /*! \brief Load/unload the HeadPose classifier
   *
   * @param[in] use
   */
  void useHeadPose( bool use );

  /*! \brief Activate the computation of the clothing colors
   *
   * @param[in] use
   */
  void useClothColors( bool use );

  /*! \brief Load/unload the Emotions classifier
   *
   * @param[in] use
   */
  void useEmotions( bool use );

  /*! \brief Store the current settings in the settings.ini file
   */
  void saveSettings();

  /*! \brief Returns the error code as follows
   *
   * Error code levels:
   *  - 0  No errors
   *  - 1X Connection errors
   *  - 2X Authentication errors
   *  - 3X Allowance errors
   *  - 4X Functional errors
   *
   *  @return A code representing an error
   */
  int getErrorCode();

  /*! \brief Returns the current error string
   *
   * @return errorString The std::string containing the error
   */
  const std::string & getErrorDescription();

  /*! \brief returns whether CrowdSight authenticated with the server or not
  *
  * @return bool value indicating whether CrowdSight is authenticated or not
  */
  bool isAuthenticated();

  /*! \brief Converts image data to an OpenCV BGR formatted matrix
   *
   * This function is intended to support the user with image format conversion. This function
   * converts an image pointed by data_array from a color space specified as input argument into
   * a standard OpenCV BGR format accepted by the SDK.
   *
   * @param[in] data_array    A pointer to the buffer containing the image data to be converted
   * @param[in] width         The width of the image
   * @param[in] height        The height of the image
   * @param[in] colorSpace    The color space of the image data (see ColorSpace)
   *
   * @return An OpenCV matrix in BGR format
   */
  cv::Mat convert( void *data_array, int width, int height, ColorSpace colorSpace );

private:
  /* CrowdSight objects cannot be copied */
  CrowdSight( const CrowdSight & );
  CrowdSight & operator=( const CrowdSight & );

  CrowdSightImpl * mCrowdSightImpl;
};

#endif /* CROWDSIGHT_H */
