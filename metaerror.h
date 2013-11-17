#ifndef ERRORTAB_H
#define ERRORTAB_H

#include <QFile>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class ErrorTab : public QWidget
{
  Q_OBJECT

public:
    
    ErrorTab(QWidget *parent = 0);
    void displayFile(QFile&);

private:

    QVBoxLayout *layout;
    QTextEdit *textEdit;
};

#endif
