#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QPainter>
#include <QSettings>
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
    connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveFrame()));
    connect(ui->seekSlider, SIGNAL(sliderMoved(int)), this, SLOT(seekFrame(int)));

    file = NULL;
    src = NULL;
    dst = NULL;

    readSettings();
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

void Dialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    QRect r, c;
    QSize s = image.size();
    c.setTopLeft(ui->picFrame->pos());
    c.setSize(ui->picFrame->size());
    r.setTopLeft(ui->picFrame->pos());
    s.scale(ui->picFrame->size(), Qt::KeepAspectRatio);
    r.setSize(s);
    r.moveCenter(c.center());

    QPainter p(this);
    p.drawImage(r, image);
}

void Dialog::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select a yuv file", QDir::homePath() + "/Desktop/*", "YUV file (*.yuv)");
    if (file)
    {
        file->close();
        delete file;
    }
    file = new QFile(filename);
    if (!file->open(QIODevice::ReadOnly)) return;

    width = ui->widthEdt->text().toInt();
    height = ui->heightEdt->text().toInt();
    writeSettings();

    if (src) delete[] src;
    if (dst) delete[] dst;
    src = new char[(int)(width * height * 1.5)];
    dst = new char[(int)(width * height * 3)];

    count = 0;
    total = (int)(file->size() / width / height / 1.5);
    ui->seekSlider->setMinimum(0);
    ui->seekSlider->setMaximum(total);
    ui->nameLbl->setText(filename);
    ui->widthEdt->setDisabled(true);
    ui->heightEdt->setDisabled(true);

    displayFrame();
}

void Dialog::nextFrame()
{  
    if (displayFrame())
    {
        ++count;
        ui->countLbl->setText(QString::number(count));
        ui->seekSlider->setValue(count);
    }
}

void Dialog::prevFrame()
{
    if (file->pos() - width * height * 3 < 0) return;
    file->seek(file->pos() - width * height * 3);
    displayFrame();
    --count;
    ui->countLbl->setText(QString::number(count));
    ui->seekSlider->setValue(count);
}

void Dialog::saveFrame()
{
    QString filename = QFileDialog::getSaveFileName(this, "Select a save path", QDir::homePath() + "/Desktop", "*.bmp");
    image.save(filename);
}

void Dialog::seekFrame(int pos)
{
    count = pos;
    ui->countLbl->setText(QString::number(count));
    file->seek(count * width * height * 1.5);
    displayFrame();
}

bool Dialog::displayFrame()
{
    if (file->read(src, (int)(width * height * 1.5)) <= 0)
        return false;
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

    image = QImage((uchar *)dst, width, height, QImage::Format_RGB888).rgbSwapped();
    update();
    return true;
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

void Dialog::writeSettings()
{
    QSettings settings("cyc", "yuvdisplay");
    settings.beginGroup("Dialog");
    settings.setValue("width", width);
    settings.setValue("height", height);
    settings.endGroup();
}

void Dialog::readSettings()
{
    QSettings settings("cyc", "yuvdisplay");
    settings.beginGroup("Dialog");
    width = settings.value("width", 1920).toInt();
    height = settings.value("height", 1080).toInt();
    settings.endGroup();
    ui->widthEdt->setText(QString::number(width));
    ui->heightEdt->setText(QString::number(height));
}
