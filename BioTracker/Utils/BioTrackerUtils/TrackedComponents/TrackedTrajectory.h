#ifndef TRACKEDOTRAJECTORY_H
#define TRACKEDOTRAJECTORY_H

#include "Interfaces/IModel/IModelTrackedTrajectory.h"
#include "QList"
#include "QString"

/**
 * This class inherits from the IModelTrackedTrajectory class and is therefor part of the Composite Pattern.
 * This class represents the Composite class.
 * This class is responsibility for the handling of Leaf objects.
 * Internaly this class uses a QList for storing Leaf object.
 *
 * Objects of this class have a QObject as parent.
 */
class TrackedTrajectory : public IModelTrackedTrajectory {
	Q_OBJECT

  public:
	TrackedTrajectory(QObject *parent = 0, QString name = "n.a.");

	// ITrackedComponent interface
public:
	void operate();

	// ITrackedObject interface
public:
	void add(IModelTrackedComponent *comp, int pos = -1) override;
	bool remove(IModelTrackedComponent *comp) override;
	void clear() override;
	IModelTrackedComponent *getChild(int index) override;
    IModelTrackedComponent *getValidChild(int index) override;
	IModelTrackedComponent *getLastChild() override;
	QList<IModelTrackedComponent*> getChildNodes() override;
	void setChildNodes(QList<IModelTrackedComponent*> n) override;


    int size() override;
    int validCount();

protected:
	QList<IModelTrackedComponent*> _TrackedComponents;

	QString name;
};

#endif // TRACKEDOTRAJECTORY_H
