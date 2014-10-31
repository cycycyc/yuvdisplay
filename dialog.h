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
    void saveFrame();
    void seekFrame(int);
    
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    Ui::Dialog *ui;
    QFile *file;
    char *src, *dst;
    QImage image;
    int width, height, count, total;

    bool displayFrame();
    void writeSettings();
    void readSettings();
};

#endif // DIALOG_H
