#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->rectBtn, &QPushButton::clicked, this, [=]{
        ui->label->setDrawingType(RECTANGLE);
    });

    connect(ui->ellipseBtn, &QPushButton::clicked, this, [=]{
        ui->label->setDrawingType(ELLIPSE);
    });

    connect(ui->lineBtn, &QPushButton::clicked, this, [=]{
        ui->label->setDrawingType(LINE);
    });

    connect(ui->polygonBtn, &QPushButton::clicked, this, [=]{
        ui->label->setDrawingType(POLYGON);
    });

    connect(ui->pipeBtn, &QPushButton::clicked, this, [=]{
        ui->label->setDrawingType(PIPE);
    });

    connect(ui->angleBtn, &QPushButton::clicked, this, [=]{
        ui->label->setDrawingType(ANGLE);
    });

    connect(ui->distanceBtn, &QPushButton::clicked, this, [=]{
        ui->label->setDrawingType(DISTANCE);
    });

//    connect(&timer, &QTimer::timeout, this, &MainWindow::slotUpdate);

//    connect(&timer, &QTimer::timeout, this, &MainWindow::slotLabelUpdate);

//    timer.start(500);

}

MainWindow::~MainWindow()
{
    delete ui;
}

