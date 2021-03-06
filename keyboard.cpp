#include "keyboard.h"
#include "ui_keyboard.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messages.h"
//#include "options.h"
#include "defines.h"

#include <QtCore>
#include <QtWidgets>
#include <QDebug>


QString keyboard::olderprop = "";
QString keyboard::newString = "";


langSimbols keyboard::simbols [] = {
    { "q", "Q", "й", "Й" },     //0
    { "w", "W", "ц", "Ц" },     //1
    { "e", "E", "у", "У" },     //2
    { "r", "R", "к", "К" },     //3
    { "t", "T", "е", "Е" },     //4
    { "y", "Y", "н", "Н" },     //5
    { "u", "U", "г", "Г" },     //6
    { "i", "I", "ш", "Ш" },     //7
    { "o", "O", "щ", "Щ" },     //8
    { "p", "P", "з", "З" },     //9
    { "[", "]", "х", "Х" },     //10
    { "{", "}", "ъ", "Ъ" },     //11
    { "a", "A", "ф", "Ф" },     //12
    { "s", "S", "ы", "Ы" },     //13
    { "d", "D", "в", "В" },     //14
    { "f", "F", "а", "А" },     //15
    { "g", "G", "п", "П" },     //16
    { "h", "H", "р", "Р" },     //17
    { "j", "J", "о", "О" },     //18
    { "k", "K", "л", "Л" },     //19
    { "l", "L", "д", "Д" },     //20
    { ";", ";", "ж", "Ж" },     //21
    { "'", "\"", "э", "Э" },     //22
    { "z", "Z", "я", "Я" },     //23
    { "x", "X", "ч", "Ч" },     //24
    { "c", "C", "с", "С" },     //25
    { "v", "V", "м", "М" },     //26
    { "b", "B", "и", "И" },     //27
    { "n", "N", "т", "Т" },     //28
    { "m", "M", "ь", "Ь" },     //29
    { "<", "<", "б", "Б" },     //30
    { ">", ">", "ю", "Ю" }      //31
};

keyboard::keyboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::keyboard)
{
    shift = false;
    secret = false;
    newString = olderprop;
    ui->setupUi(this);
    ui->textEdit->setEchoMode( QLineEdit::Normal);
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(textinput(int)) );
//    connect(ui->textEdit, SIGNAL(editingFinished()), this, SLOT(on_pushButton_13_clicked()));
    ui->labelWarning->hide();
    ui->textEdit->setText(olderprop);
//    ui->textEdit->setFocus(); // чтобы при загрузке сразу активным было окошечко с вводом параметров

    //курсор сразу в конец строки
//    QTextCursor cursor = ui->textEdit->cursor();
//    cursor.movePosition(QTextCursor::End);
//    ui->textEdit->setTextCursor(cursor);
//    int pos = ui->textEdit->cursorPosition();
//    ui->textEdit->setCursorPosition(ui->textEdit->text().size()-1);
    ui->textEdit->end(true);

    ui->textEdit->installEventFilter(this);
    ui->textEdit->deselect();

    //запись в журнал об использовании клавиатуры
    cLogger mk(pathtomessages, cLogger::UI);
    mk.addMess("Keyboard > Open", cLogger::SERVICE);
    mk.deleteLater();

    styleUnclicked = "color: rgb(255, 255, 255);"\
                    "background-color: #2c3d4d;"\
                    "border: 1px solid #1c2d3d;";

    styleClicked = "color: rgb(255, 255, 255);"\
                    "background-color: #1c2d3d;"\
                    "border: 1px solid #0c1d2d;";

    QList<QPushButton*> ButtonList = keyboard::findChildren<QPushButton*> ();
    // добавляем все кнопошки в евентфильтр
    for (int i = 0; i < ButtonList.count(); ++i) {
        QPushButton *but = ButtonList.at(i);
        but->installEventFilter(this);
        // ставим везде стайлшит
        but->setStyleSheet(styleUnclicked);
    }

    QList<QComboBox *> combos = findChildren<QComboBox *>();
    foreach(QComboBox * combo, combos)
    {
        QListView *view = new QListView(combo);
        view->setStyleSheet("QListView::item{height: 50px;}\n QListView::item:selected{background-color:rgb(44, 61, 77);}");
        QFont font;
        font.setFamily(QStringLiteral("Open Sans"));
        font.setPointSize(14);
        view->setFont(font);
        QScroller::grabGesture(view->viewport(), QScroller::LeftMouseButtonGesture);
        combo->setView(view);
        QString comboStyle = combo->styleSheet();
        QString styleAppend = "\nQComboBox::drop-down {\n	width:0px;\n }";
        combo->setStyleSheet(comboStyle + styleAppend);
//        view->deleteLater();
    }

}

keyboard::~keyboard()
{
//    MessageWrite mr ("Keyboard Close");
//    mr.deleteLater();
    cLogger mk(pathtomessages, cLogger::UI);
    mk.addMess("Keyboard > Close", cLogger::SERVICE);
    mk.deleteLater();
    delete ui;
}

void keyboard::setWarning(QString warn, bool secr)
{
//    warning = warn;
    ui->labelWarning->setText(warn);
    ui->labelWarning->show();
    secret = secr;
    ui->textEdit->setEchoMode(QLineEdit::Password);
}

QString keyboard::getcustomstring()
{
    return newString;//ui->textEdit->text();//toPlainText();
}

void keyboard::textinput(int b)
{
    QWidget *widget = QApplication::focusWidget();
    QString textwas = ui->textEdit->text();//->toPlainText();
//    QTextCursor cursor = ui->textEdit->textCursor();
    int pos = ui->textEdit->cursorPosition();//cursor.position();
//    qDebug() << "cursor" << pos;
    QPushButton *button = static_cast<QPushButton*>(widget);
//    QString textnew;
//    if(secret)
//    {
//        textnew = textwas.insert(pos, "*");
//        newString = newString.insert(pos, button->text());
//    }
//    else
//    {
//        textnew = textwas.insert(pos, button->text());
//    }
//    ui->textEdit->setText(textnew);
//    ui->textEdit->setText(newString);
        ui->textEdit->insert(button->text());
//    cursor.setPosition(pos+1);
//    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setCursorPosition(pos+1);
}

// Кнопка ОК, сохранение текущего текста
void keyboard::on_pushButton_13_clicked()
{
    newString = ui->textEdit->text();
    cLogger mk(pathtomessages, cLogger::UI);
    mk.addMess("Keyboard \"" + olderprop + "\" > \"" + ui->textEdit->displayText() + "\"",\
               cLogger::SERVICE);
    ui->labelWarning->hide();
    this->close();
}

void keyboard::on_pushButton_27_clicked()
{
    shift = !shift;
    if(shift)
    {
        ui->pushButton_27->setStyleSheet(styleClicked);
        ui->pushButton_27->setText(ui->pushButton_27->text().toUpper());
    }
    else
    {
        ui->pushButton_27->setStyleSheet(styleUnclicked);
        ui->pushButton_27->setText(ui->pushButton_27->text().toLower());
    }

    QList<QPushButton *> widgets = findChildren<QPushButton *>(); // ищем в объекте все виджеты и делаем их ресайз

    foreach(QPushButton * widget, widgets)
    {
        for(int i = 0; i < (sizeof(simbols) / sizeof(langSimbols)); i++)
        {
            QString en = simbols[i].eng;
            QString EN = simbols[i].ENG;
            QString ru = simbols[i].rus;
            QString RU = simbols[i].RUS;

            if(shift)
            {
                if (widget->property("text").toString() == en)
                    widget->setProperty("text", EN);
                if (widget->property("text").toString() == ru)
                    widget->setProperty("text", RU);
            }
            else
            {
                if (widget->property("text").toString() == EN)
                    widget->setProperty("text", en);
                if (widget->property("text").toString() == RU)
                    widget->setProperty("text", ru);
            }
        }
    }
}


void keyboard::on_pushButton_27_toggled(bool checked)
{

}

void keyboard::on_pushButton_27_clicked(bool checked)
{

}


void keyboard::on_pushButton_28_clicked()
{
    QString textwas = ui->textEdit->text();//->toPlainText();
//    QTextCursor cursor = ui->textEdit->textCursor();
    int pos = ui->textEdit->cursorPosition();//cursor.position();
    if(pos>0)
    {
        QString textnew = textwas.remove(pos-1,1);
        ui->textEdit->setText(textnew);
//        cursor.setPosition(pos-1);
//        ui->textEdit->setTextCursor(cursor);
        ui->textEdit->setCursorPosition(pos-1);
    }
}


void keyboard::on_pushButton_44_clicked()
{
    QString textwas = ui->textEdit->text();//toPlainText();
//    QTextCursor cursor = /*ui->textEdit->*/textCursor();
    int pos = ui->textEdit->cursorPosition();//cursor.position();
    QString textnew = textwas.insert(pos, ' ');
    ui->textEdit->setText(textnew);
//    cursor.setPosition(pos+1);
    ui->textEdit->setCursorPosition(pos+1);//setTextCursor(cursor);
}

void keyboard::on_comboBox_currentIndexChanged()
{
    ChangeLanguage(ui->comboBox->currentIndex() );
}

void keyboard::ChangeLanguage(int eng)
{
    QList<QPushButton *> widgets = findChildren<QPushButton *>(); // ищем в объекте все виджеты и меняем язык

    foreach(QPushButton * widget, widgets)
    {
        for(int i = 0; i < (sizeof(simbols) / sizeof(langSimbols)); i++)
        {
            QString en = simbols[i].eng;
            QString EN = simbols[i].ENG;
            QString ru = simbols[i].rus;
            QString RU = simbols[i].RUS;
            if(eng == 1) // если русский язык
            {
                if (widget->property("text").toString() == en || \
                        widget->property("text").toString() == EN)
                {
                    if(shift)
                        widget->setProperty("text", RU);
                    else
                        widget->setProperty("text", ru);
                }
            }
            else if(eng == 0)   // если англ. язык
            {
                if (widget->property("text").toString() == ru || \
                        widget->property("text").toString() == RU)
                {
                    if(shift)
                        widget->setProperty("text", EN);
                    else
                        widget->setProperty("text", en);
                }
            }

        }
    }
}


bool keyboard::eventFilter(QObject *object, QEvent *event)
{
    QKeyEvent* key = static_cast<QKeyEvent*>(event); // what key pressed
    if  (key->key() == Qt::Key_Enter) // if key == enter, then close
    {
        this->close();
    }

    if  ((key->key() >= Qt::Key_Any) &&\
         (key->key() <= Qt::Key_Z)) // if key == enter, then close
    {
//        if(secret)
//        {

//            QString str = ui->textEdit->toPlainText();
//            str.replace(QRegExp("."), "*");
//            ui->textEdit->setText(str);
//        }
    }

    if ( (event->type() == QEvent::MouseButtonPress)&& ( QString::fromLatin1(object->metaObject()->className()) == "QPushButton" ))//)inherits("QPushButton")) // ("QWidgetWindow"))
    {

        QList<QPushButton *> widgets = findChildren<QPushButton *>(); // ищем в объекте все виджеты и делаем их ресайз

        foreach(QPushButton * widget, widgets)
        {
            // ищем нажатую кнопку и подсвечиваем ее, т.е. назначаем стайлшит

            if (widget->objectName() == object->property("objectName"))
            {
                widget->setStyleSheet(styleClicked);
            }
        }
    }

    if ( (event->type() == QEvent::MouseButtonRelease)&& ( QString::fromLatin1(object->metaObject()->className()) == "QPushButton" ))//)inherits("QPushButton")) // ("QWidgetWindow"))
    {

        QList<QPushButton *> widgets = findChildren<QPushButton *>(); // ищем в объекте все виджеты и делаем их ресайз

        foreach(QPushButton * widget, widgets)
        {
            // ищем нажатую кнопку и подсвечиваем ее, т.е. назначаем стайлшит

            if (widget->objectName() == object->property("objectName"))
            {
                widget->setStyleSheet(styleUnclicked);
            }
        }
    }

    return QObject::eventFilter(object, event);
}

void keyboard::setolderproperty(QString str)
{
    this->olderproperty = str;
}

//  Кнопка ОТМЕНА, отменить изменения текста
void keyboard::on_pushButton_59_clicked()
{
    newString = olderprop;
    ui->labelWarning->hide();
    this->close();
}
