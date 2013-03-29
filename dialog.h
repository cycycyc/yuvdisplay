#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFile>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void openFile();
    void nextFrame();
    void prevFrame();
    
protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    Ui::Dialog *ui;
    QFile *file;
    char *src, *dst;
    int width, height, count;

    void displayFrame();
};

#endif // DIALOG_H
