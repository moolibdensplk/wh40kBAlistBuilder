#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPushButton>
#include "factionchoicewindow.h"

QT_USE_NAMESPACE
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void openFactionChoiceWindow();
    void closeApp();

private:
    Ui::MainWindow *ui;
    QPushButton *m_button;
    factionChoiceWindow *newFactionChoiceWindow;

};
#endif // MAINWINDOW_H