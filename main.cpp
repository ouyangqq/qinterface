
#include <QtGui/QWidget>
#include <QTextCodec>
#include <QtGui/QApplication>

#include "mainwindow.h"
#include "ui_logindlg.h"
#include "logindlg.h"
#include "myinputpanelcontext.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//GB2312//UTF-8//GB18030
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    //QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    //QTextCodec::setCodecForTr(codec);
    //QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    MyInputPanelContext *ic = new MyInputPanelContext;
    app.setInputContext(ic);

    QDialog a1;
    Ui::LoginDlg form1;
    form1.setupUi(&a1);
    LoginDlg w;
    w.show();
    if(w.exec()==QDialog::Accepted)
        {
           QMainWindow a2;
           Ui::MainWindow form2;
           form2.setupUi(&a2);
           MainWindow P;
           P.show();
           return app.exec();
        }
    else return 0;
}

