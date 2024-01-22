#include "mainwindow.h"
#include "./ui_mainwindow.h"
// ui_mainwindow.h: this file DOES NOT EXIST it is some sort of POINTER
// TO a .ui file that has the same name !!!
// without this the ui(new Ui::MainWindow) will NOT work!
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->new_list_button,&QPushButton::released, this, &MainWindow::openFactionChoiceWindow);
    connect(ui->close_app_button,&QPushButton::released, this, &MainWindow::closeApp);
    connect(ui->actionNewList, &QAction::triggered, this, &MainWindow::openFactionChoiceWindow);



}
MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::openFactionChoiceWindow() {
    newFactionChoiceWindow = new factionChoiceWindow(this);
    newFactionChoiceWindow->show();
    this->hide();
}

void MainWindow::closeApp() {
    this->close();
}
