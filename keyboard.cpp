#include "keyboard.h"
#include "ui_keyboard.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messages.h"
#include "options.h"

#include <QtCore>
#include <QtWidgets>

keyboard::keyboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::keyboard)
{
    ui->setupUi(this);
    connect(ui->buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(textinput()) );

    ui->textEdit->setText(Options::olderprop);
    ui->textEdit->setFocus(); // чтобы при загрузке сразу активным было окошечко с вводом параметров
    ui->textEdit->installEventFilter(this);

    MessageWrite mr ("Keyboard Open");
}

keyboard::~keyboard()
{
    MessageWrite mr ("Keyboard Close");
    delete ui;
}

void keyboard::on_buttonBox_accepted()
{
}

QString keyboard::getcustomstring()
{
    return ui->textEdit->toPlainText();
}

void keyboard::textinput()
{
    QWidget *widget = QApplication::focusWidget();
    QString textwas = ui->textEdit->toPlainText();
    QPushButton *button = static_cast<QPushButton*>(widget);
    QString textnew = textwas  + button->text();
    ui->textEdit->setText(textnew);
}

void keyboard::on_pushButton_13_clicked()
{
    this->close();
}

void keyboard::on_pushButton_27_clicked()
{
    QString textwas = ui->textEdit->toPlainText();
    QString textnew = textwas.remove(textwas.length()-1,1);
    ui->textEdit->setText(textnew);
}

void keyboard::on_pushButton_44_clicked()
{
    QString textwas = ui->textEdit->toPlainText();
    QString textnew = textwas.append(' ');
    ui->textEdit->setText(textnew);
}