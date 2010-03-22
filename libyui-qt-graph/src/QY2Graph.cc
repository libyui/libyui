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

  File:		QY2Graph.cc

  Author:	Arvin Schnell <aschnell@suse.de>

  Textdomain	"qt-graph"

/-*/


#include <math.h>

#include <QKeyEvent>
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>

#include "QY2Graph.h"


QY2Graph::QY2Graph(const std::string& filename, const std::string& layoutAlgorithm, QWidget* parent)
    : QGraphicsView(parent)
{
    init();

    renderGraph(filename, layoutAlgorithm);
}


QY2Graph::QY2Graph(graph_t* graph, QWidget* parent)
    : QGraphicsView(parent)
{
    init();

    renderGraph(graph);
}


QY2Graph::~QY2Graph()
{
}


void
QY2Graph::init()
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorUnderMouse);

    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    setScene(scene);
}


void
QY2Graph::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
	case Qt::Key_Plus:
	    scaleView(1.2);
	    break;
	case Qt::Key_Minus:
	    scaleView(1.0 / 1.2);
	    break;

#if 0
	case Qt::Key_Asterisk:
	    rotate(10.0);
	    break;
	case Qt::Key_Slash:
	    rotate(-10.0);
	    break;
#endif

	default:
	    QGraphicsView::keyPressEvent(event);
    }
}


void
QY2Graph::wheelEvent(QWheelEvent* event)
{
    scaleView(pow(2.0, -event->delta() / 240.0));
}


void
QY2Graph::scaleView(qreal scaleFactor)
{
    qreal f = sqrt(matrix().det());

    if (scaleFactor * f > 8.0)
	scaleFactor = 8.0 / f;
    if (scaleFactor * f < 0.1)
	scaleFactor = 0.1 / f;

    scale(scaleFactor, scaleFactor);
}


void
QY2Graph::contextMenuEvent(QContextMenuEvent* event)
{
    QY2Node* node = dynamic_cast<QY2Node*>(itemAt(event->pos()));

    if (node)
	emit nodeContextMenuEvent(event, node->name);
    else
	emit backgroundContextMenuEvent(event);
}


void
QY2Graph::mouseDoubleClickEvent(QMouseEvent* event)
{
    QY2Node* node = dynamic_cast<QY2Node*>(itemAt(event->pos()));

    if (node)
	emit nodeDoubleClickEvent(event, node->name);
}


QPointF
QY2Graph::gToQ(const pointf& p, bool upside_down) const
{
    return upside_down ? QPointF(p.x, graphRect.height() - p.y) : QPointF(p.x, -p.y);
}


QString
QY2Graph::aggetToQString(void* obj, const char* name, const QString& fallback) const
{
    const char* tmp = agget(obj, const_cast<char*>(name));
    if (tmp == NULL || strlen(tmp) == 0)
	return fallback;
    return QString::fromUtf8(tmp);
}


QColor
QY2Graph::aggetToQColor(void* obj, const char* name, const QColor& fallback) const
{
    const char* tmp = agget(obj, const_cast<char*>(name));
    if (tmp == NULL || strlen(tmp) == 0)
	return fallback;
    return QColor(tmp);
}


Qt::PenStyle
QY2Graph::aggetToQPenStyle(void* obj, const char* name, const Qt::PenStyle fallback) const
{
    const char* tmp = agget(obj, const_cast<char*>(name));
    if (tmp == NULL || strlen(tmp) == 0)
	return fallback;
    if (strcmp(tmp, "dashed") == 0)
	return Qt::DashLine;
    if (strcmp(tmp, "dotted") == 0)
	return Qt::DotLine;
    return fallback;
}


QPainterPath
QY2Graph::makeBezier(const bezier& bezier) const
{
    QPainterPath path;
    path.moveTo(gToQ(bezier.list[0]));
    for (int i = 1; i < bezier.size - 1; i += 3)
	path.cubicTo(gToQ(bezier.list[i]), gToQ(bezier.list[i+1]), gToQ(bezier.list[i+2]));
    return path;
}


void
QY2Graph::drawArrow(const QLineF& line, const QColor& color, QPainter* painter) const
{
    QLineF n(line.normalVector());
    QPointF o(n.dx() / 3.0, n.dy() / 3.0);

    QPolygonF polygon;
    polygon.append(line.p1() + o);
    polygon.append(line.p2());
    polygon.append(line.p1() - o);

    QPen pen(color);
    pen.setWidthF(1.0);
    painter->setPen(pen);

    QBrush brush(color);
    painter->setBrush(brush);

    painter->drawPolygon(polygon);
}


void
QY2Graph::renderGraph(const std::string& filename, const std::string& layoutAlgorithm)
{
    FILE* fp = fopen(filename.c_str(), "r");
    if (fp)
    {
	GVC_t* gvc = gvContext();
	if (gvc != NULL)
	{
	    graph_t* graph = agread(fp);
	    if (graph != NULL)
	    {
		if (gvLayout(gvc, graph, const_cast<char*>(layoutAlgorithm.c_str())) == 0)
		{
		    renderGraph(graph);

		    gvFreeLayout(gvc, graph);
		}
		else
		{
		    qCritical("gvLayout() failed");
		}

		agclose(graph);
	    }
	    else
	    {
		qCritical("agread() failed");
	    }

	    gvFreeContext(gvc);
	}
	else
	{
	    qCritical("gvContext() failed");
	}

	fclose(fp);
    }
    else
    {
	qCritical("failed to open %s", filename.c_str());
    }
}


QPolygonF
QY2Graph::makeShapeHelper(node_t* node) const
{
    const polygon_t* poly = (polygon_t*) ND_shape_info(node);

    if (poly->peripheries != 1)
    {
	qWarning("unsupported number of peripheries %d", poly->peripheries);
    }

    const int sides = poly->sides;
    const pointf* vertices = poly->vertices;

    QPolygonF polygon;
    for (int side = 0; side < sides; side++)
	polygon.append(gToQ(vertices[side], false));
    return polygon;
}


QPainterPath
QY2Graph::makeShape(node_t* node) const
{
    QPainterPath path;

    const char* name = ND_shape(node)->name;

    if ((strcmp(name, "rectangle") == 0) ||
	(strcmp(name, "box") == 0) ||
	(strcmp(name, "hexagon") == 0) ||
	(strcmp(name, "polygon") == 0) ||
	(strcmp(name, "diamond") == 0))
    {
	QPolygonF polygon = makeShapeHelper(node);
	polygon.append(polygon[0]);
	path.addPolygon(polygon);
    }
    else if ((strcmp(name, "ellipse") == 0) ||
	     (strcmp(name, "circle") == 0))
    {
	QPolygonF polygon = makeShapeHelper(node);
	path.addEllipse(QRectF(polygon[0], polygon[1]));
    }
    else
    {
	qWarning("unsupported shape %s", name);
    }

    return path;
}


void
QY2Graph::drawLabel(const textlabel_t* textlabel, QPainter* painter) const
{
    painter->setPen(textlabel->fontcolor);

    // Since I always just take the points from graphviz and pass them to Qt
    // as pixel I also have to set the pixel size of the font.
    QFont font(textlabel->fontname, textlabel->fontsize);
    font.setPixelSize(textlabel->fontsize);

    if (!font.exactMatch())
    {
	QFontInfo fontinfo(font);
	qWarning("replacing font \"%s\" by font \"%s\"", font.family().toUtf8().data(),
		 fontinfo.family().toUtf8().data());
    }

    painter->setFont(font);

    QString text(QString::fromUtf8(textlabel->text));
    QFontMetricsF fm(painter->fontMetrics());
    QRectF rect(fm.boundingRect(text));
    rect.moveCenter(gToQ(textlabel->pos, false));
    painter->drawText(rect.adjusted(-2, -2, +2, +2), Qt::AlignCenter, text);
}


void
QY2Graph::clearGraph()
{
    QList<QGraphicsItem*> items(scene->items());
    while (!items.isEmpty())
	delete items.takeFirst();
}


void
QY2Graph::renderGraph(graph_t* graph)
{
    clearGraph();

    if (GD_charset(graph) != 0)
    {
	qWarning("unsupported charset");
    }

    // don't use gToQ here since it adjusts the values
    graphRect = QRectF(GD_bb(graph).LL.x, GD_bb(graph).LL.y, GD_bb(graph).UR.x, GD_bb(graph).UR.y);
    scene->setSceneRect(graphRect.adjusted(-5, -5, +5, +5));

    scene->setBackgroundBrush(aggetToQColor(graph, "bgcolor", Qt::white));

    for (node_t* node = agfstnode(graph); node != NULL; node = agnxtnode(graph, node))
    {
	QPicture picture;
	QPainter painter;

	painter.begin(&picture);
	painter.initFrom(this);
	drawLabel(ND_label(node), &painter);
	painter.end();

	QY2Node* item = new QY2Node(makeShape(node), picture, node->name);

	item->setPos(gToQ(ND_coord(node)));

	QPen pen(aggetToQColor(node, "color", Qt::black));
	pen.setWidthF(1.0);
	item->setPen(pen);

	QBrush brush(aggetToQColor(node, "fillcolor", Qt::gray));
	item->setBrush(brush);

	QString tooltip = aggetToQString(node, "tooltip", "");
	if (!tooltip.isEmpty())
	{
	    tooltip.replace("\\n", "\n");
	    item->setToolTip(tooltip);
	}

	scene->addItem(item);

	for (edge_t* edge = agfstout(graph, node); edge != NULL; edge = agnxtout(graph, edge))
	{
	    const splines* spl = ED_spl(edge);
	    if (spl == NULL)
		continue;

	    for (int i = 0; i < spl->size; ++i)
	    {
		const bezier& bz = spl->list[i];

		QColor color(aggetToQColor(edge, "color", Qt::black));

		QPainterPath path(makeBezier(bz));

		QPicture picture;
		QPainter painter;

		painter.begin(&picture);
		if (bz.sflag)
		    drawArrow(QLineF(gToQ(bz.list[0]), gToQ(bz.sp)), color, &painter);
		if (bz.eflag)
		    drawArrow(QLineF(gToQ(bz.list[bz.size-1]), gToQ(bz.ep)), color, &painter);
		painter.end();

		QY2Edge* item = new QY2Edge(path, picture);

		QPen pen(color);
		pen.setStyle(aggetToQPenStyle(edge, "style", Qt::SolidLine));
		pen.setWidthF(1.0);
		item->setPen(pen);

		item->setZValue(-1.0);

		scene->addItem(item);
	    }
	}
    }
}


QY2Node::QY2Node(const QPainterPath& path, const QPicture& picture, const QString& name)
    : QGraphicsPathItem(path),
      picture(picture),
      name(name)
{
}


void
QY2Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    QGraphicsPathItem::paint(painter, option, widget);
    painter->restore();

    picture.play(painter);
}


QY2Edge::QY2Edge(const QPainterPath& path, const QPicture& picture)
    : QGraphicsPathItem(path),
      picture(picture)
{
}


QRectF
QY2Edge::boundingRect() const
{
    return QGraphicsPathItem::boundingRect().united(picture.boundingRect());
}


void
QY2Edge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    QGraphicsPathItem::paint(painter, option, widget);
    painter->restore();

    picture.play(painter);
}


#include "QY2Graph.moc"
