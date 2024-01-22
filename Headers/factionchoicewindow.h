#ifndef FACTIONCHOICEWINDOW_H
#define FACTIONCHOICEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include "listbuilderwindow.h"
QT_USE_NAMESPACE
QT_BEGIN_NAMESPACE
namespace Ui {
    class factionChoiceWindow;
}
QT_END_NAMESPACE

class factionChoiceWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit factionChoiceWindow(QWidget *parent = nullptr);
    ~factionChoiceWindow();
private slots:
    void chooseFaction();

private:
    Ui::factionChoiceWindow *ui;
    listBuilderWindow *newListBuilderWindow;

};
#endif // FACTIONCHOICEWINDOW_H