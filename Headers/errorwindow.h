#ifndef ERRORWINDOW_H
#define ERRORWINDOW_H
#include <QMainWindow>
#include <QPushButton>


QT_USE_NAMESPACE
QT_BEGIN_NAMESPACE
namespace Ui {
    class ErrorWindow;
}
QT_END_NAMESPACE

class ErrorWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit ErrorWindow(QWidget *parent = nullptr, QString errorText = "nothing");
    ~ErrorWindow();
private slots:
    void closeWindow();

private:
    Ui::ErrorWindow *ui;
    QPushButton *m_button;
};
#endif // ERRORWINDOW_H