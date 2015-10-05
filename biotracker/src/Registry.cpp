#include "Registry.h"

#include <boost/filesystem/operations.hpp>

#include <QLibrary>

#include "Exceptions.h"

namespace BioTracker {
namespace Core {

Registry::Registry() {
    m_typeByString.insert(std::make_pair("No Tracking", NoTracking));
    m_trackerByType.insert(std::make_pair(NoTracking, nullptr));
    m_stringByType.insert(std::make_pair(NoTracking, "No Tracking"));
}

bool Registry::registerTrackerType(std::string name, new_tracker_function_t f) {
    if (m_typeByString.find(name) != m_typeByString.end()) {
        throw std::invalid_argument("Tracker with same name already registered");
    }
    const TrackerType type = getNextId();
    m_typeByString.emplace(name, type);
    m_stringByType.emplace(type, name);
    m_trackerByType.emplace(type, f);

    Q_EMIT newTracker(type);

    return true;
}

void Registry::loadTrackerLibrary(const boost::filesystem::path &path) {
    typedef void (*RegisterFunction)();

    if (!boost::filesystem::exists(path)) {
        throw file_not_found("Could not find file " + path.string());
    }

    const QString str = QString::fromStdString(path.string());
    if (str.endsWith("py")) {
        std::string pyTracker = m_pyInterpreter.loadScript(path);
        Q_EMIT pythonTrackerLoaded(pyTracker);
    } else {
        QLibrary trackerLibrary(QString::fromStdString(path.string()));
        auto registerFunction = static_cast<RegisterFunction>
                                (trackerLibrary.resolve("registerTracker"));
        registerFunction();
    }
}

std::shared_ptr<TrackingAlgorithm> Registry::makeNewTracker(
    const TrackerType type, Settings &settings, QWidget *parent) const {
    const auto &it = m_trackerByType.find(type);
    if (it != m_trackerByType.cend()) {
        std::shared_ptr<TrackingAlgorithm> tracker = (it->second)(settings, parent);
        tracker->setType(type);
        return tracker;
    } else {
        return nullptr;
    }
}

TrackerType getNextId() {
    static TrackerType nextType = NoTracking + 1;

    return nextType++;
}

std::shared_ptr<TrackingAlgorithm> BioTracker::Core::Registry::getTracker(
    const std::__cxx11::string &name, Settings &s, QWidget *p) {
    if (m_pyInterpreter.hasModule(name)) {
        return m_pyInterpreter.activatePythonModule(name, s, p);
    } else {
        // TODO: implement (?)
        assert(false);
        throw std::invalid_argument("fail for now");
    }
}

}
}
