#include "BioTrackerApp.h"
#include <limits>

#include "source/util/QOpenGLContextWrapper.h"

namespace BioTracker {
namespace Core {

BioTrackerApp::BioTrackerApp()
        : m_settings()
        , m_registry(Registry::getInstance())
        , m_trackingThread(m_settings)
{
}

BioTrackerApp::~BioTrackerApp()
{
    m_trackingThread.terminate();
    m_trackingThread.wait();

}

void BioTrackerApp::initializeOpenGL(QOpenGLContext *mainContext, TextureObject &texture)
{
    auto trackingThreadContext = std::make_unique<Util::SharedOpenGLContext>(mainContext);
    trackingThreadContext->moveToThread(&m_trackingThread);

    m_trackingThread.initializeOpenGL(std::move(trackingThreadContext), texture);
}

void BioTrackerApp::openVideo(const boost::filesystem::path &path)
{
    m_trackingThread.loadVideo(path);
}

void BioTrackerApp::openImages(std::vector<boost::filesystem::path> paths)
{
    m_trackingThread.loadPictures(std::move(paths));
}

void BioTrackerApp::openCamera(int device) {
	m_trackingThread.openCamera(device);
}

void BioTrackerApp::play()
{
    m_trackingThread.enableVideoPause(false);
}

void BioTrackerApp::pause()
{
    m_trackingThread.enableVideoPause(true);
}

void BioTrackerApp::setFrame(const size_t frameNumber)
{
    m_trackingThread.setFrameNumber(frameNumber);
}

void BioTrackerApp::setTargetFps(const double fps)
{
    m_trackingThread.setFps(fps);
}

double BioTrackerApp::getTargetFps() const
{
    return std::numeric_limits<double>::infinity();
}

size_t BioTrackerApp::getNumFrames() const
{
    return m_trackingThread.getVideoLength();
}

size_t BioTrackerApp::getCurrentFrameNumber() const
{
    return m_trackingThread.getFrameNumber();
}

void BioTrackerApp::setTrackingAlgorithm(std::shared_ptr<TrackingAlgorithm> trackingAlgorithm)
{
    m_trackingThread.setTrackingAlgorithm(std::move(trackingAlgorithm));
}

void BioTrackerApp::mouseEvent(QMouseEvent *event)
{
    m_trackingThread.mouseEvent(event);
}

void BioTrackerApp::keyboardEvent(QKeyEvent *event)
{
    m_trackingThread.keyboardEvent(event);
}

} // Core
} // BioTracker
