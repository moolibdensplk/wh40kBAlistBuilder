#ifndef FACTIONDATA_H
#define FACTIONDATA_H
#include <iostream>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
/*
QT_USE_NAMESPACE
QT_BEGIN_NAMESPACE
namespace Fdb {
    class factiondata;
}
QT_END_NAMESPACE
*/

class factiondata : public QObject
{
Q_OBJECT
Q_PROPERTY(QString factionName READ getFactionName WRITE setFactionName)
public:
    //explicit factiondata(QString name);
    void setFilePath();
    QString getFilePath();
    QString const getFactionName();
    void setFactionName(const QString fname);
    void setJsonData(QString fileName);
    void setFactionLimits(QJsonObject jsonObj);
    void setFactionRules(QJsonObject jsonObj);
    void setFactionUnits(QJsonObject jsonObj);
    QJsonObject getFactionData();
    QJsonArray getFactionUnits();
    QJsonArray getFactionRules();
    QJsonObject getFactionLimits();
private:
    QString FactionFilePath;
    QString FactionName;
    QJsonObject jsonFactionData;
    QJsonObject jsonFactionLimits;
    QJsonArray jsonFactionRules;
    QJsonArray jsonFactionUnits;

};


#endif //FACTIONDATA_H