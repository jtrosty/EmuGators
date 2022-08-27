#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // I'm making edits to test how this work.
    // testing 2 to make sure branch protection works.

}

MainWindow::~MainWindow()
{
    delete ui;
}

