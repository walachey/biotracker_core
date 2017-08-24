#include "BioTrackerPlugin.h"
#include "PluginContext.h"

#include "Controller/ControllerTrackingAlgorithm.h"
#include "Controller/ControllerTrackedComponent.h"

#include "View/TrackedElementView.h"

BioTrackerPlugin::BioTrackerPlugin() {
}

IView* BioTrackerPlugin::getTrackerParameterWidget() {
    return qobject_cast<ControllerTrackingAlgorithm*> (m_TrackerController)->getTrackingParameterWidget();
}
IView *BioTrackerPlugin::getTrackerElementsWidget()
{
	return qobject_cast<ControllerTrackedComponent *> (m_ComponentController)->getTrackingElementsWidget();
}

// forwards tracked component model to coreApp
IModel* BioTrackerPlugin::getTrackerComponentModel() {
	return qobject_cast<ControllerTrackedComponent *> (m_ComponentController)->getModel();
}

void BioTrackerPlugin::setDataExporter(IModelDataExporter *exporter) {
	qobject_cast<ControllerTrackingAlgorithm*> (m_TrackerController)->setDataExporter(exporter);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(BioTrackerPlugin, BioTrackerPlugin)
#endif // QT_VERSION < 0x050000


void BioTrackerPlugin::createPlugin() {
	m_PluginContext = new PluginContext();
	m_PluginContext->createApplication();

	IController * ctr = m_PluginContext->requestController(ENUMS::CONTROLLERTYPE::COMPONENT);
	m_ComponentController = qobject_cast<ControllerTrackedComponent *>(ctr);

	IController * ctr2 = m_PluginContext->requestController(ENUMS::CONTROLLERTYPE::TRACKING);
	m_TrackerController = qobject_cast<ControllerTrackingAlgorithm *>(ctr2);

    connectInterfaces();

}

void BioTrackerPlugin::connectInterfaces() {
    ControllerTrackingAlgorithm* ctrAlg = qobject_cast<ControllerTrackingAlgorithm*> (m_TrackerController);
    QObject::connect(ctrAlg, &ControllerTrackingAlgorithm::emitCvMat, this, &BioTrackerPlugin::receiveCvMatFromController);

	QObject::connect(ctrAlg, &ControllerTrackingAlgorithm::emitTrackingDone, this, &BioTrackerPlugin::receiveTrackingDone);

	QObject::connect(ctrAlg, &ControllerTrackingAlgorithm::emitChangeDisplayImage, this, &BioTrackerPlugin::receiveChangeDisplayImage);


}

void BioTrackerPlugin::receiveCurrentFrameFromMainApp(std::shared_ptr<cv::Mat> mat, uint frameNumber) {
    qobject_cast<ControllerTrackingAlgorithm*> (m_TrackerController)->doTracking(mat, frameNumber);
}

void BioTrackerPlugin::receiveCvMatFromController(std::shared_ptr<cv::Mat> mat, QString name) {
    Q_EMIT emitCvMat(mat, name);
}

void BioTrackerPlugin::receiveTrackingDone(uint framenumber) {
    Q_EMIT emitTrackingDone(framenumber);
}

void BioTrackerPlugin::receiveChangeDisplayImage(QString str) {
	Q_EMIT emitChangeDisplayImage(str);
}

