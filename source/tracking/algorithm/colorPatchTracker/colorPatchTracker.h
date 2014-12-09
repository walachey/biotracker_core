#ifndef ColorPatchTracker_H
#define ColorPatchTracker_H

#include "cv.h"
#include "source/tracking/TrackingAlgorithm.h"

class ColorPatchTracker : public TrackingAlgorithm
{
	private:
		cv::Mat imgMask;
	public:
		ColorPatchTracker();
        ColorPatchTracker(Settings& settings, std::string& serializationPathName, QWidget *parent);
		~ColorPatchTracker(void);
		void track(ulong frameNumber, cv::Mat & frame) override;
		void paint(cv::Mat& image) override;
		void reset() override;

	public slots:
		//mouse click and move events
		void mouseMoveEvent(QMouseEvent * e) override;
		void mousePressEvent(QMouseEvent * e) override;
		void mouseReleaseEvent(QMouseEvent * e) override;
		void mouseWheelEvent(QWheelEvent * e) override;
};

#endif
