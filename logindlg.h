#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>

namespace Ui {
class LoginDlg;
}
class QPoint;       //定义一个Qpoint类

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);        //自定义一个鼠标点击事件函数
    void mouseMoveEvent(QMouseEvent *);         //自定义一个鼠标拖动事件函数

private slots:
    void on_pushButton_clicked();

private:
    Ui::LoginDlg *ui;
    QPoint dragPosition;        //定义一个QPoint的成员变量
};

#endif // LOGINDLG_H
