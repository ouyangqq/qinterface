#include "logindlg.h"
#include "ui_logindlg.h"
#include <QtGui>


LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);

    this->resize(800,480);
    ///背景加载;
    //QPixmap m_pixmapBg;
    //m_pixmapBg.load(":/res/denglu/a.png");
    //qreal width = m_pixmapBg.width(); //获得以前图片的宽和高
    // qreal height = m_pixmapBg.height();
    //int a=LoginDlg::height();
    //int b=LoginDlg::width();
    //m_pixmapBg = m_pixmapBg.scaled(b,a);
    //setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);//便可以使得窗口有最大化和最小化的按钮！
    // this->setWindowTitle ("登录窗口");


    //this->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    //this->setWindowFlags(Qt::FramelessWindowHint);//隐藏边框

    //this->setStyleSheet("background-color:rgba(0, 0, 0, 0);");//这一调色是白色，这影响到下面警告窗口的颜色，如果没有这句话的话，只在ui界面设置颜色的话，小窗口界面不会达到我们想要的颜色，这一设置很重要。


    //ui->lineEdit->setFont(QFont( "Timers" , 14 ,  QFont::Bold) );//设置lineEdit框大小，字体使用Times,18号字体，加粗
    //ui->lineEdit->setStyleSheet("background-color:white");//设置lineEdit框背景色为白色


    //ui->lineEdit_2->setFont(QFont( "Timers" , 14 ,  QFont::Bold) );
    //ui->lineEdit_2->setStyleSheet("background-color:white");


    //ui->pushButton->setFont(QFont( "Timers" , 18 ,  QFont::Bold) );//登录按钮设置
    //ui->pushButton->setStyleSheet("background-color:white");
    //ui->pushButton->setStyleSheet("border-image: url(://res/image/whilte.png)");


    //ui->pushButton_3->setFont(QFont( "Timers" , 18 ,  QFont::Bold) );//登录按钮设置
    //ui->pushButton_3->setStyleSheet("background-color:white");
    //ui->pushButton_3->setStyleSheet("border-image: url(://res/image/whilte.png)");
}
LoginDlg::~LoginDlg()
{
    delete ui;
}
void LoginDlg::on_pushButton_clicked()
{
    ui->lineEdit->setFocus();
    if(ui->lineEdit->text()!=tr("9")&&ui->lineEdit_2->text()!=tr("0"))
    {

        //QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);//弹出串口格式
        QMessageBox::information(NULL, tr("警告"), tr("用户名与密码错误"), QMessageBox::Yes, QMessageBox::Yes);
        ui->lineEdit->clear();                      //清除用户名
        ui->lineEdit_2->clear();                 //清除密码
        ui->lineEdit->setFocus();              //鼠标回到用户名栏
    }
    else if(ui->lineEdit->text()!=tr("9")&&ui->lineEdit_2->text()==tr("0"))
    {
        QMessageBox::information(NULL, tr("警告"), tr("用户名错误"), QMessageBox::Yes, QMessageBox::Yes);
        ui->lineEdit->clear();                      //清除用户名
        ui->lineEdit->setFocus();              //鼠标回到用户名栏
    }
    else if(ui->lineEdit->text()==tr("9")&&ui->lineEdit_2->text()!=tr("0"))
    {
        QMessageBox::information(NULL, tr("警告"), tr("密码错误"), QMessageBox::Yes, QMessageBox::Yes);
        ui->lineEdit_2->clear();                 //清除密码
        ui->lineEdit_2->setFocus();              //鼠标回到用户名栏
    }
    else
    {
        accept();
    }

}
void LoginDlg::paintEvent(QPaintEvent *)
{
   //QPainter painter(this);
   //painter.drawPixmap(0,0,this->width(),this->height(),QPixmap("://res/denglu/a.png"));
}
void LoginDlg::mouseMoveEvent(QMouseEvent * event)
{
    /*
    if (event->buttons() == Qt::LeftButton) //当满足鼠标左键被点击时。
    {
         move(event->globalPos() - dragPosition);//移动窗口
         event->accept();
    }*/
}
void LoginDlg::mousePressEvent(QMouseEvent * event)
{
    /*
    if (event->button() == Qt::LeftButton) //点击左边鼠标
    {
         dragPosition = event->globalPos() - frameGeometry().topLeft();//// 由鼠标相对屏幕的坐标减去窗口左上角坐标，其实得到的就是鼠标相对窗口的位置
         //globalPos()获取根窗口的相对路径，frameGeometry().topLeft()获取主窗口左上角的位置
         event->accept();   //鼠标事件被系统接收
    }
    //if (event->button() == Qt::RightButton)
    //{
        // close();
    //}
    */
}

