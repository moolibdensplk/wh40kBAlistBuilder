#include "listbuilderwindow.h"
#include <QMainWindow>
#include <iostream>
#include <QString>
#include <QDebug>
#include <QTableWidgetItem>
#include <QLineEdit>
#include "factiondata.h"
#include "./ui_listbuilderwindow.h"
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QtPrintSupport>


listBuilderWindow::listBuilderWindow(QWidget *parent, QString f_name)
        : QMainWindow(parent) , ui(new Ui::listBuilderWindow) , f_db(new factiondata)
{
    ui->setupUi(this);
    connect(ui->closeListButton, &QPushButton::released, this, &listBuilderWindow::closeApp);
    connect(ui->clearListButton, &QPushButton::released, this, &listBuilderWindow::clearList);
    connect(ui->addToListButton, &QPushButton::released, this, &listBuilderWindow::addUnit);
    connect(ui->validateListButton, &QPushButton::released, this, &listBuilderWindow::validateList);
    connect(ui->removeSelectedUnitButton, &QPushButton::released, this, &listBuilderWindow::removeUnit);
    connect(ui->saveListButton,&QPushButton::released,this,&listBuilderWindow::exportPDF);
    connect(ui->actionCloseList,&QAction::triggered, this, &listBuilderWindow::closeApp);

    //qDebug() << "DEBUG: value of *f_name at class init: " << f_name.toStdString() <<;
    f_db ->setFactionName(f_name);
    f_db -> setFilePath();
    this->populateFactionData();
    this->populateUnitChoiceComboBox(f_db->getFactionUnits());
    //set total point cost at 0:
    this->setPointCost(0);
    QString pointLimit = QString::number(this->getPointCost());
    // make sure list validation is false
    this->setListValidated(false);

    ui->listStatusBarLabel->setText("Total point cost: " + pointLimit + "/500 points.");
    ui->listStatusBarLabel->setStyleSheet("QLabel { background-color : grey; color : black; }");
    ui->validationLabel->setStyleSheet("QLabel { background-color : red; color : white; }");
    ui->validationLabel->setText("LIST NOT VALIDATED");
}

listBuilderWindow::~listBuilderWindow(){
    delete ui;
}

void listBuilderWindow::populateFactionData() {
    QString path;
    //try to load the JSON data from the file to the relevant property in the factiondata class
    f_db->setJsonData(f_db -> getFilePath());
    f_db->setFactionRules(f_db->getFactionData());
    f_db->setFactionLimits(f_db->getFactionData());
    f_db->setFactionUnits(f_db->getFactionData());

}

void listBuilderWindow::setUpFaction(QString faction) {
    f_db->setFactionName(faction);
}
void listBuilderWindow::closeApp() {
    //delete ui;
    this->close();

}
void listBuilderWindow::addUnit() {
    QString unitName;
    QJsonArray factionData;
    QJsonArray unitDependencies;
    QString unitDependenciesItem;
    QJsonObject unitData;
    QString unitType;
    //QString unitSizesItem[3];
    bool choiceValidated = false;

    int dataIndex = ui->unitChoiceComboBox->currentIndex();
    unitName = ui->unitChoiceComboBox->itemData(dataIndex).toString();
    factionData = f_db->getFactionUnits();

    unitData = factionData[dataIndex].toObject();
    choiceValidated = this->validateUnitRequirements(unitData);
    unitType = unitData["unit_type"].toString();
    unitDependencies = unitData["requires"].toArray();
    int unitCost = unitData["unit_cost"].toInt();
    QString unitCostStr;
    QJsonArray unitSizes = unitData["unit_sizes"].toArray();

    if(unitDependencies.isEmpty()){
        unitDependenciesItem = "none";
    }else {
        if(unitDependencies.size()==1){
            unitDependenciesItem = "[ " + unitDependencies[0].toString() + " ]";
        }else{
            unitDependenciesItem ="[ ";
            for(int i=0;i<unitDependencies.size();i++){
                unitDependenciesItem = unitDependenciesItem + unitDependencies[i].toString() + ", ";
            }
            unitDependenciesItem = unitDependenciesItem + " ]";
        }
    }
    int totalCostSoFar = this->getPointCost();
    if(choiceValidated == true && this->canUnitFit(unitCost, totalCostSoFar) == true) {

        int secondSizeVal = unitSizes[1].toInt();
        int unitSizeStep = secondSizeVal - unitSizes.first().toInt();
        QSpinBox *unitSizeSpinBox = new QSpinBox;
        unitSizeSpinBox->setRange(unitSizes.first().toInt(), unitSizes.last().toInt());
        unitSizeSpinBox->setSingleStep(unitSizeStep);
        unitSizeSpinBox->findChild<QLineEdit*>()->setReadOnly(true);
        int rowCount = ui->unitListTableWidget-> rowCount();
        ui->unitListTableWidget->insertRow(rowCount);
        QTableWidgetItem *item0 =  new QTableWidgetItem(unitName);
        QTableWidgetItem *item1 =  new QTableWidgetItem(unitType);
        QTableWidgetItem *item2 =  new QTableWidgetItem(unitDependenciesItem);
        unitCostStr = QString::number(unitCost);
        QTableWidgetItem *item4 =  new QTableWidgetItem(unitCostStr);

        ui->unitListTableWidget->setItem(rowCount, 0, item0);
        ui->unitListTableWidget->setItem(rowCount, 1, item1);
        ui->unitListTableWidget->setItem(rowCount, 2, item2);
        ui->unitListTableWidget->setCellWidget(rowCount,3,unitSizeSpinBox);
        ui->unitListTableWidget->setItem(rowCount, 4, item4); //not
        int unitSize = unitSizeSpinBox->value();

        // QT slots cannot take arguments !
        // QT Slots can only take arguments passed by Signal ! (onValueChanged sends an int "value", so the slot could only take that value !!)
        // Therefore, to workaround it, you have to use a LAMBDA as a slot, to catch the PREVIOUS value too
        int baseUnitCost = unitCostStr.toInt();
        connect(unitSizeSpinBox, &QSpinBox::valueChanged,[prevSpinValue=unitSizeSpinBox->value(), this, baseUnitCost](int value) mutable {
            // run a helper function
            listBuilderWindow::updateUnitCostValue(prevSpinValue, value, baseUnitCost);
            prevSpinValue = value;

        });
        // Update the total point cost and the label.
        int currentTotal = this->getPointCost();
        int newTotal = currentTotal + unitCost;
        this->setPointCost(newTotal);
        ui->listStatusBarLabel->setText("Total point cost: " + QString::number(newTotal) + "/500 points.");
        // adding a unit invalidates list validation
        this->setListValidated(false);
        ui->validationLabel->setStyleSheet("QLabel { background-color : red; color : white; }");
        ui->validationLabel->setText("LIST NOT VALIDATED");
        // WORKS !!!
    }else if(choiceValidated == false){
        QString addErrMsg = "[ERROR]: Unit: " + unitName + ", has missing dependencies that need to be added first: " + unitDependenciesItem + "\n";
        this->displayErrorMsg(addErrMsg);
    }
    /*else if(this->canUnitFit(unitCost, totalCostSoFar) == false) {
        QString addErrMsg = "[ERROR]: Unit: " + unitName + ", does not fit into remaining points. " + unitDependenciesItem + "\n";
        this->displayErrorMsg(addErrMsg);
    }*/
}

void listBuilderWindow::clearList() {
    while(ui->unitListTableWidget->rowCount()>0){
        ui->unitListTableWidget->removeRow(0);
    }
    //update the costs
    this->emptyValidationErrors();
    int totalCost = 0;
    this->setPointCost(totalCost);
    this->setListValidated(false);
    ui->listStatusBarLabel->setText("Total point cost: " + QString::number(totalCost) + "/500 points." );
    ui->validationLabel->setStyleSheet("QLabel { background-color : red; color : white; }");
    ui->validationLabel->setText("LIST NOT VALIDATED");
    // clear all validation errors

}

void listBuilderWindow::validateList() {

    QJsonObject unitObj; // <this is empty atm ...
    QJsonObject listValidationResults;
    QJsonObject checkResults;

    QJsonArray unitObjects;
    QJsonObject failedChecks;
    failedChecks.empty();
    unitObjects.empty();
    checkResults.empty();
    listValidationResults.empty();
    unitObj.empty();
    // every time RESET the list validation status
    this->setListValidated(false);

    int armyListSize = ui->unitListTableWidget->rowCount();
    if (armyListSize > 0) {
        for (int x=0;x<armyListSize;x++) {
            unitObj = {
                    {"unit_name", ui->unitListTableWidget->item(x,0)->text()},
                    {"unit_type", ui->unitListTableWidget->item(x,1)->text()},
                    {"unit_cost", ui->unitListTableWidget->item(x,4)->text()}
            };
            unitObjects.append(unitObj);
        }

        checkResults = this->checkFactionLimits(unitObjects);

                foreach(QString checkName, checkResults.keys()){
                if (checkResults[checkName] == "FAIL") {
                    //assign only failed checks
                    failedChecks[checkName] = checkResults[checkName];
                }
            }

        QString eMsg = "[ERROR]: LIST VALIDATION ERRORS: \n";
        if (failedChecks.size() > 0 ){
                    foreach(QString checkName, failedChecks.keys()){
                    eMsg = eMsg + "[ERROR]: " + checkName + ": " + failedChecks[checkName].toString() + "\n";
                }
            this->displayErrorMsg(eMsg);
            this->setListValidated(false);
        } else {
            //qDebug() << "[DEBUG]: validate() failedChecks = " << failedChecks;
            //qDebug() << "[DEBUG]: validate() failedChecks size = " << failedChecks.size();
            ui->validationLabel->setStyleSheet("QLabel { background-color : green; color : white; }");
            ui->validationLabel->setText("LIST VALIDATED. OK TO EXPORT AS PDF NOW !");

            this->setListValidated(true);
        }

    } else {
        ui->validationLabel->setStyleSheet("QLabel { background-color : red; color : white; }");
        ui->validationLabel->setText("LIST NOT VALIDATED");
        this->setListValidated(false);
    }

};

void listBuilderWindow::removeUnit() {
    int tableWidgetSize = ui->unitListTableWidget->rowCount();
    if (tableWidgetSize > 0) {
        int selectedRow = ui->unitListTableWidget->currentRow();
        if(selectedRow>=0){
            QString unitToRemove = ui->unitListTableWidget->item(selectedRow,0)->text();
            if (listBuilderWindow::isDependency(unitToRemove) == false) {
                int removedUnitCost = ui->unitListTableWidget->item(selectedRow,4)->text().toInt();
                int totalPCost = this->getPointCost();
                totalPCost = totalPCost - removedUnitCost;
                this->setPointCost(totalPCost);
                ui->unitListTableWidget->removeRow(selectedRow);
                ui->listStatusBarLabel->setText("Total point cost: " + QString::number(totalPCost) + "/500 points. List Validated: FALSE");
                // every time you remove a unit from list - reset list validation
                this->setListValidated(false);
                ui->validationLabel->setStyleSheet("QLabel { background-color : red; color : white; }");
                ui->validationLabel->setText("LIST NOT VALIDATED");
            } else {
                QString errMsg = "[ERROR]: CANNOT REMOVE unit: " + unitToRemove +
                                 "\n while its dependant unit is in the list.\n";
                this->displayErrorMsg(errMsg);
            }
        }
    }
}

void listBuilderWindow::populateUnitChoiceComboBox(QJsonArray unitList) {
    for(auto unit : unitList) {
        QString unitStringName;
        int unitPointCost;
        QString unitPointCostQstr;
        QString unitChoiceLine;
        unitStringName = unit.toObject()["unit_name"].toString();
        unitPointCost = unit.toObject()["unit_cost"].toInt();
        // convert to QString
        unitPointCostQstr = QString::number(unitPointCost);
        unitChoiceLine = unitStringName + ", " + unitPointCostQstr;
        ui->unitChoiceComboBox->addItem(unitChoiceLine, unitStringName);

    }
}

bool listBuilderWindow::validateUnitRequirements(QJsonObject unitData) {
    QJsonArray uDependencies = unitData["requires"].toArray();
    int tableItemCount = ui->unitListTableWidget->rowCount();
    QList<QString> addedUnits;
    for(int i=0;i<tableItemCount;i++){
        addedUnits.append(ui->unitListTableWidget->item(i,0)->data(Qt::DisplayRole).toString()); // wrong type
    }
    // loop through the list of dependencies, returning true for the first met dependency.
    if(uDependencies.size()>0) {
        for (int i = 0; i <= uDependencies.size(); i++) {
            if (addedUnits.contains(uDependencies[i].toString())) {
                return true;
            } else if (uDependencies[i].toString() == "none") {
                return true;
            }
        }
    } else {
        // if unit being added has no requirements - return true
        return true;
    }
    // for anything else: false
    return false;
}


void listBuilderWindow::updateUnitCostValue(int bSize, int nSize, int bCost) {
    int currentRow = ui->unitListTableWidget->currentRow();
    int currentCost = ui->unitListTableWidget->item(currentRow, 4)->text().toInt();
    int newCost;
    QString newCostStr;
    int totalCost = this->getPointCost();
    if (bSize < nSize) {
        if (this->canUnitFit(bCost, totalCost) == true) {
            newCost = currentCost + bCost;
            newCostStr = QString::number(newCost);
            QTableWidgetItem *newCostItem = new QTableWidgetItem(newCostStr);
            ui->unitListTableWidget->setItem(currentRow,4,newCostItem);
            totalCost = totalPointCost + bCost;
            this->setPointCost(totalCost);
            ui->listStatusBarLabel->setText("Total point cost: " + QString::number(totalCost) + "/500 points");
            // everytime you increase unit size: reset list validation
            this->setListValidated(false);
        }
        // there seem to be a bug, where if adding a unit with a spin button,
        // if you reach the points limit, it allows you to go up a step (if you click enough time),
        // and sets base cost to 0, or even negative......
        // the below code does not fix / prevent it... :( to be fixed in the future.

    } else {
        newCost = currentCost - bCost;

        newCostStr = QString::number(newCost);
        QTableWidgetItem *newCost = new QTableWidgetItem(newCostStr);
        ui->unitListTableWidget->setItem(currentRow,4,newCost);
        totalCost = totalPointCost - bCost;
        this->setPointCost(totalCost);
        ui->listStatusBarLabel->setText("Total point cost: " + QString::number(totalCost) + "/500 points");
        // everytime you decrease unit size: reset list validation
        this->setListValidated(false);
    }

}

int listBuilderWindow::getPointCost() {
    return this->totalPointCost;
}

void listBuilderWindow::setPointCost(int points) {
    this->totalPointCost = points;
}

bool listBuilderWindow::isDependency(QString removedUnitName) {
    int totalRows = ui->unitListTableWidget->rowCount();
    for (int i=0;i<totalRows;i++){
        QString dependenciesSTR = ui->unitListTableWidget->item(i,2)->text();
        if (dependenciesSTR.contains(removedUnitName)) {
            return true;
        }
    }
    return false;
}

bool listBuilderWindow::canUnitFit(int unitPrice, int totalCostSoFar) {
    int remainingPoints = 500 - totalCostSoFar;
    if (unitPrice <= remainingPoints) {
        return true;
    }
    return false;
}


int listBuilderWindow::countUnitTypesInList(QString uType) {

    QJsonArray unitTypes;
    QJsonArray typeDuplicates;

    int totalRows = ui->unitListTableWidget->rowCount();
    for (int i=0;i<totalRows;i++){
        unitTypes.append(ui->unitListTableWidget->item(i, 1)->text());
    }
    for (int j=0;j<unitTypes.size();j++) {
        if (unitTypes[j] == uType) {
            typeDuplicates.append(uType);
        }
    }
    return typeDuplicates.size();
}

QString listBuilderWindow::checkTypeLimit(QString uType) {
    QJsonObject fLimits = f_db->getFactionLimits();

    QString limitType = "max_" + uType;
    int limit = fLimits[limitType].toInt();
    int amountOfUnitTypeInList = this->countUnitTypesInList(uType);
    if (amountOfUnitTypeInList <= limit) {
         return "PASS";
    }
    return "FAIL";
}

QString listBuilderWindow::checkDuplicateLimit(QString dupLimitType, QString uName) {
    QJsonObject fLimits = f_db->getFactionLimits();
    int dupNameCount = 0;
    QJsonArray dupNames;
    int listSize = ui->unitListTableWidget->rowCount();
    for (int a=0;a<listSize;a++) {
        if(ui->unitListTableWidget->item(a,0)->text() == uName){
            dupNames.append(uName);
        }
    }

    dupNameCount = dupNames.size();
    if (fLimits[dupLimitType] == false && dupNameCount > 1 && dupLimitType != "battleline") {
        return "FAIL";
    }
    return "PASS";
}

QJsonObject listBuilderWindow::checkFactionLimits(QJsonArray list) {
    QJsonArray allUnitTypes;
    QSet<QString> uniqueTypes;
    QJsonArray allUnitNames;
    QSet<QString>  uniqueNames;
    QString typeCheck;
    QString dupCheck;
    QJsonObject factionLimitsResults;
    QJsonObject dupCheckUnitList;


    for (int i=0;i<list.size();i++){
        allUnitNames.append(list[i].toObject()["unit_name"]);
        allUnitTypes.append(list[i].toObject()["unit_type"]);
    }

    for (int a=0;a<allUnitTypes.size();a++) {
        uniqueTypes.insert(allUnitTypes[a].toString());
    }
    for (int a=0;a<allUnitNames.size();a++) {
        uniqueNames.insert(allUnitNames[a].toString());
    }

    for (auto i = allUnitTypes.begin(), end = allUnitTypes.end(); i != end; ++i){
        typeCheck = this->checkTypeLimit(i->toString());
        setValidationErrors("max_"+i->toString(),typeCheck);
    }

    for (int i=0;i<list.size();i++){
    QString tmpName = list[i].toObject()["unit_name"].toString();
    QString tmpType = list[i].toObject()["unit_type"].toString();
    dupCheckUnitList[tmpName] = tmpType;

    }

    foreach(QString uName, uniqueNames){
        QString tmpType = "duplicate_"+dupCheckUnitList[uName].toString();
        dupCheck = this->checkDuplicateLimit(tmpType,uName);
        setValidationErrors(tmpType,dupCheck);
    }
    factionLimitsResults = getValidationErrors();
    return factionLimitsResults;
}

void listBuilderWindow::setValidationErrors(QString valType, QString valError) {
    listValidationErrors[valType] = valError;
}

QJsonObject listBuilderWindow::getValidationErrors(){
    return listValidationErrors;
}

void listBuilderWindow::displayErrorMsg(QString msg) {
    newErrorUi = new ErrorWindow(this, msg);
    newErrorUi->show();

}

void listBuilderWindow::emptyValidationErrors() {
    listValidationErrors = {};

}

void listBuilderWindow::setListValidated(bool bValue) {
    listValidated = bValue;
}

bool listBuilderWindow::getListValidated() {
    return listValidated;
}

void listBuilderWindow::exportPDF() {
    QJsonObject listToExport;
    QString saveDir = std::getenv("HOME");
    QString pdfFileName;
    bool isValidated = getListValidated();

    QString factionName = f_db->getFactionName();
    QString htmlTableRows = "";
    int totalCost = this->getPointCost();
    int tableWidgetSize = ui->unitListTableWidget->rowCount();
    QJsonArray factionRulesJson = f_db->getFactionRules();
    QString factionRulesHtml;

    if (tableWidgetSize > 0 && isValidated == true) {
        pdfFileName = QFileDialog::getSaveFileName(this,"Choose file name to save",saveDir, ".pdf");
        for (int i=0;i< tableWidgetSize; i++) {
            QString name = ui->unitListTableWidget->item(i,0)->text();
            QString type = ui->unitListTableWidget->item(i,1)->text();
            QWidget *sizeWidget = ui->unitListTableWidget->cellWidget(i,3);
            QSpinBox* sizeBox = static_cast<QSpinBox*>(sizeWidget);
            QString size = QString::number(sizeBox->value());
            QString cost = ui->unitListTableWidget->item(i,4)->text();
            htmlTableRows = htmlTableRows + "<tr><td>" + name + "</td><td><p>" + type + "</td><td>" + size + "</td><td>" + cost + "</td></tr>";
        }
        for(int j=0;j<factionRulesJson.size();j++) {
            factionRulesHtml = factionRulesHtml + "<li> " + factionRulesJson[j].toString() + "</li>";
        }

        if(!pdfFileName.isEmpty()&& !pdfFileName.isNull()){
            QString htmlArmyList =
                    "<html><head></head><body><h1 align=center> WH40K Boarding Actions Army List</h1>"
                    "<BR>"
                    "<h3 align=left> Faction: " + factionName  + "</h3>"
                    "<h3 align left> Points limit: 500 points </h3>"
                    "<table align=justify  style=\"border-collapse: collapse; width: 80%; font-size: 80%;\" cellpadding=5 border=1><tr>"
                    "<td >Unit Name</td><td >Unit Type</td><td>Unit Size</td><td >Unit Cost</td></tr>"
                    + htmlTableRows +
                    "</table></div>"
                    "<h4 align=left> Total point cost: " + QString::number(totalCost) + " points</h4>"
                    "<h4> Faction Rules </h4>"
                    "<ul>" + factionRulesHtml + "</ul></body></html>";

            QTextDocument pdfDocument;
            pdfDocument.setHtml(htmlArmyList);
            QPrinter printer(QPrinter::PrinterResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(pdfFileName);
            printer.setPageMargins(QMarginsF(15, 15, 15, 15));
            pdfDocument.print(&printer);
        }
    } else if (ui->unitListTableWidget->rowCount() == 0){
        QString eMessage = "[ERROR]: YOUR LIST IS EMPTY !!! NOTHING TO EXPORT";
        this->displayErrorMsg(eMessage);
    } else if(isValidated == false){
        QString eMessage = "[ERROR]: You need to validate your list first.\n Your list is currently not validated!";
        this->displayErrorMsg(eMessage);
    }
}