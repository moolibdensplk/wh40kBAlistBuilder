#include "errorwindow.h"
#include "./ui_errorwindow.h"

//
// Created by Marcin Stangel on 16/01/2024.
//
ErrorWindow::ErrorWindow(QWidget *parent, QString errorTxt)
        : QMainWindow(parent), ui(new Ui::ErrorWindow)
{
    ui->setupUi(this);
    ui->errorMsgLabel->setText(errorTxt);
    connect(ui->errorCloseButton,&QPushButton::released, this, &ErrorWindow::closeWindow);
}

ErrorWindow::~ErrorWindow(){
    delete ui;
}

void ErrorWindow::closeWindow() {
    // close the window.
    this->close();

}

