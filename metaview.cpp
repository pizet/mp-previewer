#include "metaview.h"

MetaView::MetaView(const QString& path, QWidget *parent) : QWidget(parent)
{
  QImage metapostImage;
  metapostImage = loadImage(path); 
  
  QPixmap metapostPixmap;
  metapostPixmap.convertFromImage(metapostImage);

  metapostScene = new QGraphicsScene(this);
  pixmapItem = metapostScene->addPixmap(metapostPixmap);

  metapostView = new QGraphicsView(metapostScene);

  layout = new QVBoxLayout(this);
  layout->addWidget(metapostView);
}

void MetaView::updateMetaView(const QString& path)
{
  QImage metapostImage;
  metapostImage = loadImage(path);

  int horizontalScrollBarPos;
  horizontalScrollBarPos = metapostView->horizontalScrollBar()->value();
  
  int verticalScrollBarPos;
  verticalScrollBarPos = metapostView->verticalScrollBar()->value();
  
  pixmapItem->setPixmap(QPixmap::fromImage(metapostImage));
  metapostScene->setSceneRect(pixmapItem->sceneBoundingRect());
  metapostView->horizontalScrollBar()->setValue(horizontalScrollBarPos);
  metapostView->verticalScrollBar()->setValue(verticalScrollBarPos);
}

QImage MetaView::loadImage(const QString& path)
{
  Poppler::Document *pdfDocument;
  pdfDocument = Poppler::Document::load(path);
  pdfDocument->setRenderHint(Poppler::Document::Antialiasing);
  pdfDocument->setRenderHint(Poppler::Document::TextAntialiasing);

  Poppler::Page *pdfPage;
  pdfPage = pdfDocument->page(0);

  QImage metapostImage;
  metapostImage = pdfPage->renderToImage(144.0, 144.0, -1, -1, -1, -1);

  return metapostImage;
}
