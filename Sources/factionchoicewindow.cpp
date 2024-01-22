#include "factionchoicewindow.h"
#include <QMainWindow>
#include <iostream>
#include <QString>
#include "./ui_factionchoicewindow.h"
#include "listbuilderwindow.h"

factionChoiceWindow::factionChoiceWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::factionChoiceWindow)
{
    ui->setupUi(this);
    connect(ui->factionChoiceNextButton, &QPushButton::released, this, &factionChoiceWindow::chooseFaction);
}

factionChoiceWindow::~factionChoiceWindow(){
    delete ui;
}

void factionChoiceWindow::chooseFaction() {
    QString selectedFaction = ui->factionChoiceBox->currentText();
    // initialise new window with relevant data
    newListBuilderWindow = new listBuilderWindow(this, selectedFaction);
    newListBuilderWindow->setUpFaction(selectedFaction);
    newListBuilderWindow->setProperty("factionName",selectedFaction);
    newListBuilderWindow->show();
    this->hide();
   //works !!

}
