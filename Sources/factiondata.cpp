#include "factiondata.h"
#include <iostream>
#include <QString>
#include <QDebug>
#include <QFile>
#include <filesystem>
#include <CoreFoundation/CoreFoundation.h>

void factiondata::setFilePath() {

    QString dirName = "/DataFiles";
    QString appDir = "/wh40kbalistbuilder";
    QString homePath = std::getenv("HOME");
    QString dataDirPath = homePath + appDir + dirName;

    CFBundleRef myAppBundle = CFBundleGetMainBundle();

    if(FactionName.toStdString() == "TAU Empire") {
        FactionFilePath = dataDirPath + "/tau_empire.json";
    }else if(FactionName.toStdString() == "Chaos Daemons") {
        FactionFilePath = dataDirPath + "/chaos_daemons.json";
    }
}

QString factiondata::getFilePath() {
    return FactionFilePath;
}

void factiondata::setFactionName(const QString fname) {
    FactionName=fname;
}

const QString factiondata::getFactionName() {
    return FactionName;
}

void factiondata::setJsonData(QString fileName) {
    QFile factionDataFile;
    factionDataFile.setFileName(fileName);
    if(!factionDataFile.open(QIODevice::ReadOnly)){

        qDebug() << "[ERROR] Json file: " + fileName +" couldn't be opened/found !";
    }
    QByteArray byteArray;
    byteArray = factionDataFile.readAll();
    factionDataFile.close();
    QJsonParseError parseError;
    QJsonDocument jsonDoc;
    jsonDoc = QJsonDocument::fromJson(byteArray, &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qDebug() << "[ERROR]: Parse error at " << parseError.offset << ": " << parseError.errorString().toStdString();
    }
    jsonFactionData = jsonDoc.object();
}

void factiondata::setFactionUnits(QJsonObject jsonDataObj) {
    QJsonValue listOfUnitsValue = jsonDataObj["Boarding Actions Units"];
    jsonFactionUnits = listOfUnitsValue.toArray();
}

void factiondata::setFactionLimits(QJsonObject jsonDataObj) {
    jsonFactionLimits = jsonDataObj.value("Boarding Actions Limits").toObject();
}
void factiondata::setFactionRules(QJsonObject jsonDataObj) {
    jsonFactionRules = jsonDataObj.value("Boarding Actions Rules Modifications").toArray();
}

QJsonObject factiondata::getFactionData() {
    return jsonFactionData;
}

QJsonArray factiondata::getFactionUnits() {
    return jsonFactionUnits;
}

QJsonArray factiondata::getFactionRules() {
    return jsonFactionRules;
}
QJsonObject factiondata::getFactionLimits() {
    return jsonFactionLimits;
}