/****************************************************************************
  **
  ** This file is part of the BioTracker Framework
  ** by Andreas Jörg
  **
  ****************************************************************************/


#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include "Interfaces/IModel/IModel.h"
#include "QThread"
#include "Model/MediaPlayerStateMachine/MediaPlayerStateMachine.h"

#include <ctime>
#include <chrono>

/**
 * The MediaPlayer class is an IModel class an part of the MediaPlayer component. This class creats a MediaPlayerStateMachine object and moves it to a QThread.
 * The MediaPlayer class controlls and represents the results of the MediaPlayerStateMachine. The communication between a MediaPlayer object and a MediaPlayerStateMachine object is done by
 * Qts SIGNALS and SLOTS. This is due to the fact that SIGNALS and SLTOS are thread safe. A disadvantage is that SIGNALS and SLOTS are slower then normal function calls.
 *
 * The constructor of this class is responsible for connecting to the MediaPlayerStateMachine.
 */
class MediaPlayer : public IModel {
    Q_OBJECT
  public:
    MediaPlayer(QObject* parent = 0);
    ~MediaPlayer();

  Q_SIGNALS:
    /**
    * Emit the path to a video stream. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void loadVideoStream(QString str);
    /**
    * Emit the path to pictures. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void loadPictures(std::vector<boost::filesystem::path> files);
    /**
    * Emit the camera device number. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void loadCameraDevice(int i);

    /**
    * Emit a frame number. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void goToFrame(int frame);
    /**
    * Emit the next frame command. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void nextFrameCommand();
    /**
    * Emit previous frame command. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void prevFrameCommand();
    /**
    * Emit the play command. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void playCommand();
    /**
    * Emit stop command. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void stopCommand();
    /**
    * Emit the pause command. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
    */
    void pauseCommand();

    /**
     * This SIGNAL will be emmited if a state operation should be executed. This signal will be received by the MediaPlayerStateMachine which runns in a separate Thread.
     */
    void runPlayerOperation();

    /**
     * This SIGNAL will send a cv::Mat and a name to the MediaPlayer controller class. This controller will give the data to the TextureObject component.
     */
    void renderCurrentImage(std::shared_ptr<cv::Mat> mat, QString name);
    /**
     * This SIGNAL is only emmited if Tracking Is Active. The PluginLoader component will receive the cv::Mat and the current frame number.
     */
    void trackCurrentImage(std::shared_ptr<cv::Mat> mat, uint number);


  public:
    void setTrackingActive();
    void setTrackingDeactive();

    bool getPlayState();
    bool getForwardState();
    bool getBackwardState();
    bool getStopState();
    bool getPauseState();

    bool getTrackingState();

    size_t getTotalNumberOfFrames();
    size_t getCurrentFrameNumber();
    double getFpsOfSourceFile();
    double getCurrentFPS();
    QString getCurrentFileName();
    std::shared_ptr<cv::Mat> getCurrentFrame();


  public Q_SLOTS:
    /**
     * MediaPlayer will receive the current playerParameters from the MediaPlayerStateMachine.
     */
    void receivePlayerParameters(playerParameters* param);

    /**
     * If the MediaPlayerStateMachine is finished with executing the current state it will trigger this SLOT.
     */
    void receivePlayerOperationDone();

    /**
     * If a BioTracker Plugin is done with executing its tracking algorithm this SLOT will be triggerd. As soon as this SLOT is triggered, the MediaPlayerStateMachine will be
     * advised to execute the next state.
     */
    void receiveTrackingOperationDone();


  private:
    QPointer< QThread > m_PlayerThread;
    QPointer< MediaPlayerStateMachine > m_Player;

    IPlayerState* m_CurrentPlayerState;
    IPlayerState* m_NextPlayerState;

    QMap<IPlayerState::PLAYER_STATES, IPlayerState*> m_States;

    size_t m_TotalNumbFrames;
    size_t m_CurrentFrameNumber;
    double m_fpsOfSourceFile;
    double m_currentFPS;
    QString m_CurrentFilename;
    std::shared_ptr<cv::Mat> m_CurrentFrame;

    bool m_Play;
    bool m_Forw;
    bool m_Back;
    bool m_Stop;
    bool m_Paus;


    bool m_TrackingIsActive;


    QString m_NameOfCvMat = "Original";


    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
};

#endif // MEDIAPLAYER_H