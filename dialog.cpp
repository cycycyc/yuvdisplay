#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QKeyEvent>
extern "C" {
#include "yuv2rgb/yuv2rgb.h"
}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->openBtn, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->nextBtn, SIGNAL(clicked()), this, SLOT(nextFrame()));
    connect(ui->prevBtn, SIGNAL(clicked()), this, SLOT(prevFrame()));

    file = NULL;
    src = NULL;
    dst = NULL;
}

Dialog::~Dialog()
{
    delete ui;

    if (file)
    {
        file->close();
        delete file;
    }
    if (src) delete[] src;
    if (dst) delete[] dst;
}

void Dialog::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select a yuv file", "/Users/chenyc/misc/*", "YUV file (*.yuv)");
    if (file)
    {
        file->close();
        delete file;
    }
    file = new QFile(filename);
    file->open(QIODevice::ReadOnly);
    width = ui->widthEdt->text().toInt();
    height = ui->heightEdt->text().toInt();
    if (src) delete[] src;
    if (dst) delete[] dst;
    src = new char[(int)(width * height * 1.5)];
    dst = new char[(int)(width * height * 3)];
    count = 0;
    ui->nameLbl->setText(filename);
    ui->widthEdt->clearFocus();
    ui->heightEdt->clearFocus();
    displayFrame();
}

void Dialog::nextFrame()
{
    ++count;
    displayFrame();
}

void Dialog::prevFrame()
{
    --count;
    file->seek(file->pos() - width * height * 3);
    displayFrame();
}

void Dialog::displayFrame()
{
    file->read(src, (int)(width * height * 1.5));
    yuv420_2_rgb888((uint8_t *)dst,                                 // dst addr
                    (uint8_t *)src,                                 // Y addr
                    (uint8_t *)src + (int)(width * height),         // U addr
                    (uint8_t *)src + (int)(width * height * 1.25),  // V addr
                    width,                                          // width
                    height,                                         // height
                    width,                                          // Y span
                    width / 2,                                      // UV span
                    width * 3,                                      // dst span
                    yuv2rgb565_table,
                    0);

    ui->picLbl->setPixmap(QPixmap::fromImage(QImage((uchar *)dst, width, height, QImage::Format_RGB888).rgbSwapped()));
    ui->countLbl->setText(QString::number(count));
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Left:
        prevFrame();
        break;
    case Qt::Key_Right:
        nextFrame();
        break;
    default:
        break;
    }
}
