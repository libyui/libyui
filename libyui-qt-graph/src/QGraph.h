/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						   (c) SuSE Linux GmbH |
\----------------------------------------------------------------------/

  File:		QGraph.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/


#ifndef QGraph_h
#define QGraph_h

#include <gvc.h>
#include <string>

#include <QKeyEvent>
#include <QWheelEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QToolButton>
#include <QPicture>


class QGraph : public QGraphicsView
{
    Q_OBJECT

public:

    QGraph(QWidget* parent, const std::string& filename, const std::string& layoutAlgorithm);

    QGraph(QWidget* parent, graph_t* graph);

    virtual ~QGraph();

    virtual void renderGraph(const std::string& filename, const std::string& layoutAlgorithm);
    virtual void renderGraph(graph_t* graph);

protected:

    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent* event);
    void scaleView(qreal scaleFactor);

    /**
     * Transform graphviz point into Qt QPointF.
     */
    QPointF gToQ(const point& p) const;

private:

    void init();

    QGraphicsScene* scene;
    QToolButton* corner;

    QSizeF size;

    void arrow(QPainterPath& path, const QLineF& line, const QString& type);

};


class Node : public QGraphicsRectItem
{

public:

    Node(const QRectF& rect);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    QPicture picture;

};


#endif // QGraph_h
