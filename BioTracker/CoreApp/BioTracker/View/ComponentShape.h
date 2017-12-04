#ifndef COMPONENTSHAPE_H
#define COMPONENTSHAPE_H

#include "QGraphicsObject" 
#include "Interfaces/IView/IViewTrackedComponent.h"
#include "Interfaces/IModel/IModelTrackedComponent.h"
#include "qpainter.h"
#include "qgraphicsitem.h"
#include "View/TrackedComponentView.h"
#include "Interfaces/IModel/IModelTrackedTrajectory.h"
#include "Model/CoreParameter.h"


class ComponentShape : public QGraphicsObject
{
	Q_OBJECT

	public:
		//Constructor
		ComponentShape(QGraphicsObject* parent = 0, IModelTrackedTrajectory* trajectory = 0, int id = -1);

		QRectF boundingRect() const override;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
		QPainterPath shape() const override;
		bool advance();

		bool updatePosition(uint framenumber);
		void updateAttributes();
		IModelTrackedTrajectory* getTrajectory();
		void setPermission(std::pair<ENUMS::COREPERMISSIONS, bool> permission);
		int getId();
		bool isSwappable();
		bool isRemovable();
		void trace();
		void setMembers(CoreParameter* coreParams);

		//public member
		int m_currentFramenumber;

	signals:
		void emitRemoveTrajectory(IModelTrackedTrajectory* trajectory);
		void emitRemoveTrackEntity(IModelTrackedTrajectory* trajectory);
		void emitMoveElement(IModelTrackedTrajectory* trajectory, QPoint pos);
		void broadcastMove();

	public Q_SLOTS:
		void changeBrushColor();
		void changePenColor();
		void changeBrushColor(QColor color);
		void changePenColor(QColor color);
		bool removeTrack();
		bool removeTrackEntity();
		void markShape(int penwidth = 0);
		void unmarkShape();

		//Tracing
		void receiveTracingLength(int tracingLength);
		void receiveTracingStyle(QString style);
		void receiveTracingSteps(int steps);
		void receiveTracingTimeDegradation(QString timeDegradation);
		void receiveTracerProportions(float proportion);
		void receiveTracerOrientationLine(bool toggle);
		//Visual
		void receiveAntialiasing(bool toggle);
		//Dimensions
		void receiveDimensions(int width, int height);
		void setDimensionsToDefault();
		void receiveToggleOrientationLine(bool toggle);


	protected:
		QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

		void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
		void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
		void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	private:

		//Member
		QGraphicsObject *m_parent;
		std::map<ENUMS::COREPERMISSIONS, bool> m_permissions;
		bool m_pMovable;
		bool m_pRemovable;
		bool m_pSwappable;
		bool m_antialiasing;
		IModelTrackedTrajectory* m_trajectory;
		QList<QPolygonF> m_polygons;
		int m_id;
		int m_z;
		int m_w;
		int m_wDefault = 20;
		int m_h;
		int m_hDefault = 20;
		bool m_useDefaultDimensions;
		float m_rotation;
		QColor m_penColor;
		QColor m_penColorLast;
		QColor m_brushColor;
		bool m_marked;
		int m_penWidth;
		Qt::PenStyle m_penStyle;
		bool m_dragged;
		QPoint m_mousePressPos;
		QString m_tracingStyle;
		int m_tracingLength;
		int m_tracingSteps;
		QString m_tracingTimeDegradation;
		QGraphicsRectItem* m_tracingLayer;
		float m_tracerProportions;
		bool m_tracingOrientationLine;
		bool m_orientationLine;
};


#endif // COMPONENTSHAPE_H