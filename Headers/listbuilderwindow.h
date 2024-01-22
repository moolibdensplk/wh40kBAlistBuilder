#ifndef LISTBUILDERWINDOW_H
#define LISTBUILDERWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include "factiondata.h"
#include <QFile>
#include <QSpinBox>
#include <QFileDialog>
#include "errorwindow.h"

QT_USE_NAMESPACE
QT_BEGIN_NAMESPACE
namespace Ui {
    class listBuilderWindow;
}
QT_END_NAMESPACE

class listBuilderWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit listBuilderWindow(QWidget *parent = nullptr, QString f_name = "nothing");
    ~listBuilderWindow();
    void setUpFaction(QString aFaction);
    void populateFactionData();
    void populateUnitChoiceComboBox(QJsonArray unitList);
    bool validateUnitRequirements(QJsonObject unitData);
    void updateUnitCostValue(int pSize, int nSize, int baseCost);
    void setPointCost(int points);
    int getPointCost();
    bool isDependency(QString unitName);
    bool canUnitFit(int unitPrice, int totalCostSoFar);
    QJsonObject checkFactionLimits(QJsonArray simplifiedList);
    QString checkTypeLimit(QString unitType);
    int countUnitTypesInList(QString unitType);
    QString checkDuplicateLimit(QString limitType, QString unitName);
    void setValidationErrors(QString valType, QString valError);
    QJsonObject getValidationErrors();
    void displayErrorMsg(QString msg);
    void emptyValidationErrors();
    void setListValidated(bool bValue);
    bool getListValidated();

private slots:
    void closeApp();
    void exportPDF();
    void validateList();
    void clearList();
    void addUnit();
    void removeUnit();

private:
    int totalPointCost;
    bool listValidated;
    QJsonObject listValidationErrors;
    Ui::listBuilderWindow *ui;
    QString factionName;
    factiondata *f_db;
    ErrorWindow *newErrorUi;

};
#endif // LISTBUILDERWINDOW_H

