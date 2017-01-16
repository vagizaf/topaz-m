#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

private slots:

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();

    void checkboxchange(QString newstring);
    void savesettings();

private:

    Ui::Options *ui;
};

#endif // OPTIONS_H
