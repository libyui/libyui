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

  File:		QGraph.cc

  Author:	Arvin Schnell <aschnell@suse.de>

  Textdomain	"qt-graph"

/-*/


#include <math.h>

#include "QGraph.h"


QGraph::QGraph(QWidget* parent, const std::string& filename, const std::string& layoutAlgorithm)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);

    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    renderGraph(filename, layoutAlgorithm);
    setScene(scene);
}


QGraph::QGraph(QWidget* parent, graph_t* graph)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);

    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    renderGraph(graph);
    setScene(scene);
}


QGraph::~QGraph()
{
}


void
QGraph::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Plus:
            scaleView(1.2);
            break;
        case Qt::Key_Minus:
            scaleView(1.0 / 1.2);
            break;
        default:
            QGraphicsView::keyPressEvent(event);
    }
}


void
QGraph::wheelEvent(QWheelEvent* event)
{
    scaleView(pow(2.0, -event->delta() / 240.0));
}


void
QGraph::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.05 || factor > 10)
	return;

    scale(scaleFactor, scaleFactor);
}


QPointF
QGraph::gToQ(const point& p) const
{
    QPointF tmp(p.x, p.y);
    return QPointF(tmp.x(), size.height() - tmp.y());
}


QString
aggetToQ(void* obj, char* name, const char* fallback)
{
    const char* tmp = agget(obj, name);
    if (tmp == NULL || tmp[0] == '\0')
	return fallback;
    else
	return QString(tmp);
}


void
QGraph::arrow(QPainterPath& path, const QLineF& line, const QString& type)
{
    if (type == "normal")
    {
	QLineF n(line.normalVector());
	QPointF o(n.dx() / 2.0, n.dy() / 2.0);

	path.moveTo(line.p1());
	path.moveTo(line.p1() + o);
	path.lineTo(line.p2());
	path.lineTo(line.p1() - o);
	path.closeSubpath();
    }
    else if (type == "empty")
    {
	QLineF n(line.normalVector());
	QPointF o(n.dx() / 2.0, n.dy() / 2.0);

	path.moveTo(line.p1());
	path.moveTo(line.p1() + o);
	path.lineTo(line.p2());
	path.lineTo(line.p1() - o);
	path.closeSubpath();
    }
    else if (type == "open")
    {
	QLineF n(line.normalVector());
	QPointF o(n.dx() / 2.0, n.dy() / 2.0);

	path.moveTo(line.p1());
	path.lineTo(line.p2());

	path.moveTo(line.p1() + o);
	path.lineTo(line.p2());
	path.lineTo(line.p1() - o);
    }
    else if (type == "none")
    {
	path.moveTo(line.p1());
	path.lineTo(line.p2());
    }
    else
    {
	// yuiError() << "unknown arrow type " << type.toStdString() << endl;
    }
}


void
QGraph::renderGraph(const std::string& filename, const std::string& layoutAlgorithm)
{
    GVC_t* gvc = gvContext();

    FILE* fp = fopen(filename.c_str(), "r");
    if (fp)
    {
	graph_t* graph = agread(fp);

	char* tmp = strdup(layoutAlgorithm.c_str());
	gvLayout(gvc, graph, tmp);
	free(tmp);

	renderGraph(graph);

	gvFreeLayout(gvc, graph);
	agclose(graph);
    }
    else
    {
	// yuiError() << "failed to open " << filename << endl;
    }

    gvFreeContext(gvc);
}


void
QGraph::renderGraph(graph_t* graph)
{
    // don't use gToQ here since it adjusts the values
    QRectF rect(GD_bb(graph).LL.x, GD_bb(graph).LL.y, GD_bb(graph).UR.x, GD_bb(graph).UR.y);
    const qreal border = 20.0;
    scene->setSceneRect(rect.adjusted(-border, -border, border, border));

    size = rect.size();


    QPen pen1("red");
    pen1.setWidthF(1);
    QBrush brush1("yellow");
    QPen pen2("blue");
    pen2.setWidthF(1);


    for (Agnode_t* node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node))
    {
	QRectF rect(0.0, 0.0, 72.0*ND_width(node), 72.0*ND_height(node));
	rect.moveCenter(gToQ(ND_coord_i(node)));

	QGraphicsItem* item = NULL;

	if (strcmp(ND_shape(node)->name, "ellipse") == 0)
	{
	    Node* shape = new Node(rect, ND_label(node)->text);
	    scene->addItem(shape);
	    shape->setPen(pen1);
	    shape->setBrush(brush1);
	    shape->setLabelPen(QPen(Qt::blue));
	    item = shape;
	}
	else
	{
	    Node* shape = new Node(rect, ND_label(node)->text);
	    scene->addItem(shape);
	    shape->setPen(pen1);
	    shape->setBrush(brush1);
	    shape->setLabelPen(QPen(Qt::black));
	    item = shape;
	}

	const char* tooltip = agget(node, "tooltip");
	if (tooltip && tooltip[0] != '\0')
	    item->setToolTip(tooltip);

	for (Agedge_t* edge = agfstedge(graph, node); edge != NULL; edge = agnxtedge(graph, edge, node))
	{
	    const splines* spl = ED_spl(edge);

	    for (int i = 0; i < spl->size; i++)
	    {
		const bezier& bz = spl->list[i];

		QPainterPath path;

		if (bz.sflag)
		    arrow(path, QLineF(gToQ(bz.list[0]), gToQ(bz.sp)), aggetToQ(edge, "arrowhead", "normal"));

		path.moveTo(gToQ(bz.list[0]));
		for (int j = 1; j < bz.size-1; j += 3)
		    path.cubicTo(gToQ(bz.list[j]), gToQ(bz.list[j+1]), gToQ(bz.list[j+2]));

		if (bz.eflag)
		    arrow(path, QLineF(gToQ(bz.list[bz.size-1]), gToQ(bz.ep)), aggetToQ(edge, "arrowtail", "normal"));

		QGraphicsPathItem* shape = scene->addPath(path);

		shape->setPen(pen2);
		shape->setZValue(-1.0);
	    }
	}
    }
}


Node::Node(const QRectF& rect, const QString& label)
    : QGraphicsRectItem(rect),
      label(label)
{
}


void
Node::setLabelPen(const QPen& tmp)
{
    labelPen = tmp;
}


void
Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QGraphicsRectItem::paint(painter, option, widget);
    painter->setPen(labelPen);
    painter->drawText(rect(), Qt::AlignCenter | Qt::AlignHCenter, label);
}


#include "QGraph.moc"
