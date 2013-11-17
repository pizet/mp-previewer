#include <QByteArray>

#include "metaerror.h"

ErrorTab::ErrorTab(QWidget *parent) : QWidget(parent)
{
  textEdit = new QTextEdit;
  textEdit->setReadOnly(true);

  layout = new QVBoxLayout(this);
  layout->addWidget(textEdit);
}

void ErrorTab::displayFile(QFile& file)
{
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QByteArray byteArray = file.readAll();
    QString text(byteArray);
    textEdit->setPlainText(text);
  }
}
