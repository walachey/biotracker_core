#ifndef BioTracker_H
#define BioTracker_H

#include <memory>

// QT 
#include <QtWidgets/QMainWindow>
#include <QtCore/QString>
#include <QTemporaryFile>

//Bio Tracker
#include "source/tracking/algorithm/algorithms.h"

#include "source/ui_BioTracker.h"

Q_DECLARE_METATYPE(cv::Mat)
class TrackingThread;
class VideoView;
class TrackingAlgorithm;
class Settings;

class BioTracker: public QMainWindow
{
	Q_OBJECT

public:
	BioTracker(Settings &settings, QWidget *parent = nullptr,  Qt::WindowFlags flags = 0);
    ~BioTracker();
public slots:
	// open file browser for video in/out
	void browseVideo();
	// open file browser for picture in/out
	void browsePicture();
	//checks current state (stopped,paused or playing)
	//and then sends appropriate signal to tracking thread
	void runCapture();
	// stops video thread
	void stopCapture();
	// next frame 
	void stepCaptureForward();
	// previous frame
	void stepCaptureBackward();
	//pause video thread;
	void pauseCapture();
	//change video playback speed
	void changeFps(int fps);
	//different tracking algorithm was selected
    void trackingAlgChanged(Algorithms::Type trackingAlg);
    void trackingAlgChanged(QString trackingAlgStr);
	//switch pan&zoom mode
	void switchPanZoomMode();


	// SLOTS FOR TRACKING THREAD: 	
	void updateFrameNumber(int frameNumber);	
	void drawImage(cv::Mat image);
	void showFps(double fps);
	void invalidFile();

	/**
	 * Print proivided message to the GUI message area.
	 * @param: message, the message to print.
	 * @return: void.
	 */
	void printGuiMessage(std::string message, MSGS::MTYPE mType = MSGS::MTYPE::NOTIFICATION);

	/**
	 * Sets the algorithm used for tracking
	 * @param: trackingAlgId, the id of the algorithm.
	 */
	
	//void setTrackingAlg(int trackingAlgId);

	//void changeCurrentFrame();
	void changeCurrentFramebySlider();
	void changeCurrentFramebySlider(int SliderAction);
	void changeCurrentFramebyEdit();

	void takeScreenshot();

	//returns true if video is in paused, false otherwise
	bool isVideoPaused();

private:
	Ui::BioTrackerClass ui;

/*	Rectification _rectification;
*/
    std::unique_ptr<TrackingThread> _trackingThread;

	Settings& _settings;
	bool _videoPaused;
	bool _videoStopped;
	bool _isPanZoomMode;
	int _currentFrame;
	QIcon _iconPause;
	QIcon _iconPlay;
	//disable or enable buttons for video navigating
	void setPlayfieldPaused(bool enabled);
	void init();
	void initGui();	
	void initConnects();
	void initCapture();
	void initAlgorithms();
	void initPicture(QStringList filenames);
    void connectTrackingAlg(std::shared_ptr<TrackingAlgorithm> tracker);
	void setPlayfieldEnabled(bool enabled);

    std::shared_ptr<TrackingAlgorithm> _tracker;
    std::map<Algorithms::Type, QTemporaryFile> _serializationTmpFileMap;

	//Containers to put in chosen algorithm specific ui stuff
	QVBoxLayout *_vboxParams;
	QVBoxLayout *_vboxTools;
    std::shared_ptr<QWidget> _paramsWidget;
    std::shared_ptr<QWidget> _toolsWidget;
	
signals:
	// tell tracking thread to grab next frame
	void nextFrameReady(bool);

	// tell tracking thread to grab specific frame
	void changeFrame(int);

	//tell Tracker to stop
	void videoStop();

	// tell tracking thread to pause/unpause video
	void videoPause(bool);	

	//tell tracking thread to grab next frame
	void grabNextFrame();

	//tell tracking thread to change playback speed
	void fpsChange(double fps);
	
	//enable max playback speed
	void enableMaxSpeed (bool enabled);

	//change tracking algorithm
    void changeTrackingAlg(std::shared_ptr<TrackingAlgorithm>);

};

#endif // BioTracker_H
