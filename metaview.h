#ifndef METAVIEW_H
#define METAVIEW_H

#include <poppler-qt4.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QList>
#include <QProcess>
#include <QScrollBar>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

class MetaView : public QWidget
{
public:
  
  MetaView(const QString&, QWidget *parent = 0);
  void updateMetaView(const QString&);

private:
  QImage loadImage(const QString&);
  
  QGraphicsPixmapItem *pixmapItem;
  QGraphicsScene *metapostScene;
  QGraphicsView *metapostView;
  QVBoxLayout *layout;
};

#endif
