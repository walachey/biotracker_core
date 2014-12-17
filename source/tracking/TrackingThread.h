#ifndef TrackingThread_h
#define TrackingThread_h

#include <atomic>
#include <memory>

#include <opencv2/opencv.hpp>

#include <QThread>

#include "source/tracking/TrackingAlgorithm.h"
#include "source/utility/MutexWrapper.h"

class Settings;

class TrackingThread : public QThread
{
	Q_OBJECT
public:
	TrackingThread(Settings &settings);
	~TrackingThread(void);
	
	/**
	* Starts the video thread.
	*/
	void loadVideo(const std::string& filename);

	/**
	* Loads in pictures instead of a video
	*/
	void loadPictures(const std::vector<std::string>&& filenames);

	/**
	* Reset
	*/
	void resetTracker();

	/**
	* Checks if thread can handle next frame.
	* @return true if it can, false otherwise.
	*/
	bool isReadyForNextFrame();

	/**
	* Gets the length of the video.
	* @return the video length.
	*/
	int getVideoLength();

	/**	
	* @return  current fps setting
	*/
	double getFps() const;

	/**
	 * @brief stops the currently running thread
	 */
	void stop();

private:
	Mutex _captureActiveMutex;
	Mutex _frameNumberMutex;
	Mutex _readyForNexFrameMutex;
	Mutex _trackerMutex;

	/** 
	* Video handling.
	*/
	// For reading the video file or video stream
	cv::VideoCapture _capture;	
	//current frame
	cv::Mat _frame;
	
	//defines whether to use pictures as source or a video
	bool _captureActive GUARDED_BY(_captureActiveMutex);
	bool _readyForNextFrame GUARDED_BY(_readyForNexFrameMutex);
	int _frameNumber GUARDED_BY(_frameNumberMutex);
	std::atomic<bool> _videoPause;
	bool _trackerActive;
	double _fps;
	double _runningFps;
	bool _maxSpeed;
	GUIPARAM::MediaType _mediaType;
	
	//contains filenames of all pictures that were selected by user
	std::vector<std::string> _pictureFiles;

	Settings &_settings;

	std::shared_ptr<TrackingAlgorithm> _tracker GUARDED_BY(_trackerMutex);

	cv::Mat getPicture(size_t index);


	/**
	 * Increments the current frame number by 1 frame.
	 */
	void incrementFrameNumber();
	
	/**
	* Checks if thread is in pause state.
	* @return true if paused, false otherwise.
	*/
	bool isVideoPause() const;

	/**
	* Checks if the tracker is on.
	* @return true if tracker is on, false otherwise.
	*/ 
	bool isCaptureActive();

	/**
	* Set the capture in active or in-active state
	* @param enabled if true capture will be activated, false otherwise
	*/
	void enableCapture(bool enabled);

	/**
	* Initializes the reading capture.
	*/
	void initCaptureForReadingVideoOrStream();

	/**
	* sends frame and everything else that is needed to selected
	* tracking algorithm
	*/
	void doTracking();

	/** 
	* thread running method.
	*/
	void run();

public slots:
	void enableHandlingNextFrame(bool nextFrame);
	void enableVideoPause(bool videoPause);

	/**
	* Stops the video.
	*/
	void stopCapture();

	/**
	* Sets the current frame number.
	* @param frameNumber specifies the current frame number.
	*/
	void setFrameNumber(int frameNumber);

	/**
	* Gets current frame number.
	* @return the current frame number.
	*/
	int getFrameNumber();

	/**
	* Moves one frame forward
	*/
	void nextFrame();

	/**
	* change framerate
	*/
	void setFps(double fps);

	/**
	* enable maximum playback speed
	*/
	void setMaxSpeed (bool enabled);

	void setTrackingAlgorithm( std::shared_ptr<TrackingAlgorithm> TrackingAlgorithm );

	void doTrackingAndUpdateScreen();

signals:
	/**
	* Signals when a tracking sequence is done.
	* @param image send the image to draw,
	*/
	void trackingSequenceDone(cv::Mat image);

	/**
	* emit current frame number.
	* @param frameNumber the current frame number.
	*/
	void newFrameNumber(int frameNumber);

	/**
	* singal to gui that video is paused
	*/
	void invalidFile();

	/**
	* send a message to the GUI.
	*/
	void notifyGUI(std::string message, MSGS::MTYPE type = MSGS::MTYPE::NOTIFICATION);
	//send current fps to gui
	void sendFps(double fps);
};

#endif // !TrackingThread_h
