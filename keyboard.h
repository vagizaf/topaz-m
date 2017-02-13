#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QDialog>
#include <mainwindow.h>

#include <channel1.h>
namespace Ui {
class keyboard;
}

class keyboard : public QDialog
{
    Q_OBJECT

public:
    explicit keyboard(QWidget *parent = 0);

    ~keyboard();


    bool onlydigits;

private slots:
    void on_buttonBox_accepted();
    void textinput();

    void on_pushButton_13_clicked();

    void on_pushButton_27_clicked();

public slots:
    QString getcustomstring();
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::keyboard *ui;
};

#endif // KEYBOARD_H
