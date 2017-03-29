#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class QwtPlot;
class QwtPlotCurve;

#include <QMainWindow>
#include <QtGui>
//#include <QtCore>
#include "qcustomplot.h"
#include "qextserial/qextserialport.h"
//???????????????????TIME_OUT???????????????????????????????????????????
#define TIME_OUT 10

//?????????????????????????????????????????????????????????????????????,200ms
#define OBO_TIMER_INTERVAL 3000

//??????????????????????????????????????????????????????????????????????????????????????????TIP_FILE_SIZE??????????????????????????????????????????????????????????????????????????????????????????????????
#define TIP_FILE_SIZE 5000
//???????????????????????????????????????????????????????????????????????MAX_FILE_SIZE???????????????????
#define MAX_FILE_SIZE 10000

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void startInit();
    int				m_timeAreaWidth;
    void temc_Init();
    double              Y1;
    double              Y2;
    double              Y3;
    double              Y4;
    double              Y5;
    double              Y6;
    double              Y7;
    double              Y8;

    //void mouseMoveEvent(QMouseEvent *e)

    //void q_init_ui();
    void init_ui();
    void init_strength_ui();
    void zhenduan_init();
    void biaoge_init();
    unsigned short int CRC_16(unsigned char * pchMsg, unsigned short int wDataLen);
    void load_config_ini();
    void make_sure();
    QDateTime		m_baseTime;
    double m_timeData[3600];
    QwtPlotCurve*		m_curve;
    QwtPlotCurve*		m_curve1;
    QwtPlotCurve*		m_curve2;
    QwtPlotCurve*		m_curve3;
    QwtPlotCurve*		m_curve4;
    QwtPlotCurve*		m_curve5;
    QwtPlotCurve*		m_curve6;
    QwtPlotCurve*		m_curve7;
    void timerEvent( QTimerEvent * );
    void timer_sampleEvent( QTimerEvent * );
    void timer_serialportEvent( QTimerEvent * );
    void setupRealtimecurve(QCustomPlot *customPlot);
    void setupRealtimecurve2(QCustomPlot *customPlot2);
    void setupRealtimecurve3(QCustomPlot *customPlot3);
    void setupRealtimecurve4(QCustomPlot *customPlot4);
    void setupRealtimecurve5(QCustomPlot *customPlot5);
    void setupRealtimecurve6(QCustomPlot *customPlot6);
    void setupRealtimecurve7(QCustomPlot *customPlot7);
    void setupRealtimecurve8(QCustomPlot *customPlot8);
    QVector<QPointF>m_vp;
    QVector<QPointF>m_vp1;
    QVector<QPointF>m_vp2;
    QVector<QPointF>m_vp3;
    QVector<QPointF>m_vp4;
    QVector<QPointF>m_vp5;
    QVector<QPointF>m_vp6;
    QVector<QPointF>m_vp7;
public slots:
    void timerUpDate();
    void timer_sampleUpDate();
    void timer_serialportUpDate();
    void timerUpDate1();
//void legendChecked( const QVariant &itemInfo, bool on );
//void legendChecked1( const QVariant &itemInfo, bool on );

protected:
    void paintEvent(QPaintEvent *);
    //void mouseMoveEvent(QMouseEvent *);
    //void mousePressEvent(QMouseEvent *);
    //void changeEvent(QEvent *e);
    void setActionsEnabled(bool status);
    void setComboBoxEnabled(bool status);
private:
    Ui::MainWindow *ui;
    //AboutDialog aboutdlg;
    QextSerialPort *myCom;
    QTimer *obotimer;
    QTimer *timer_sample;//数据读取定时器
    QTimer *timer_serialport;//串口通信定时器
    unsigned int timerdly;
    unsigned int obotimerdly;
    QString write2fileName;    //????????????????????????????????????????????????????????????????????

    QPixmap m_pixmapBg;
    QAction *m_AactionAboutQt;
    QMenu *m_menu;

    //QPoint m_pointStart;
    QPoint m_pointPress;

    QVector <QPushButton* > m_vecBtn;
    ///??????????????????????????????????????????;
    void initData();
    ///??????????????????????????????????????;
    void setNomalStyle();
    ///???????????????????????????????????????????;
    void setCurrentWidget();

    QString bkPicName;//主窗口背景图片的名称
    QPoint normalTopLeft;//正常显示的窗口左上角坐标

    //设置表格的内容
    QTableWidgetItem *item1_0, *item1_1,*item1_2, *item1_3,*item1_4, *item1_5,*item1_6, *item1_7;//设置温度对应的表格内容
    QTableWidgetItem *item2_0, *item2_1,*item2_2, *item2_3,*item2_4, *item2_5,*item2_6, *item2_7;//设置温差对应的表格内容
    QTableWidgetItem *item3_0, *item3_1,*item3_2, *item3_3,*item3_4, *item3_5,*item3_6, *item3_7;//设置热负荷对应的表格内容
    QTableWidgetItem *item4_0, *item4_1,*item4_2, *item4_3,*item4_4, *item4_5,*item4_6, *item4_7;//设置热流强度对应的表格内容

private slots:
    void on_menuBtn_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();
    void on_pushButton_19_clicked();

    //void on_pushButton_9_clicked();

    void on_nomalBtn_clicked();

    void anniuyangshi();
    //void curveinstall();

    //void on_actionWriteToFile_triggered();
    void on_actionCleanPort_triggered();
    //void on_actionLoadfile_triggered();
    //void on_delayspinBox_valueChanged(int );
    //void on_actionAdd_triggered();
    //void on_actionExit_triggered();
    //void on_actionSave_triggered();
    //void on_obocheckBox_clicked();
    void on_actionClearBytes_triggered();
    //void on_actionAbout_triggered();
    //void on_actionClose_triggered();
    void on_actionOpen_triggered();   //打开串口

    void on_clearUpBtn_clicked();
    void on_open_Sciport_clicked();
    void on_sendmsgBtn_clicked();
    void read_temperature();
    void read_battery_dl();
    void read_signal_strenth();
    void on_tclear_clicked();
    void readMyCom();
    void sendMsg();
    void send_serial_data();
    void showSkinWidget();
    void setPicName(QString picName);
    void on_close_Sciport_clicked();
    void on_start_sample_clicked();
    void on_stop_sample_clicked();
    void on_delayspinBox_editingFinished();
    void on_verticalSlider_bj_T_valueChanged(int value);

    void on_verticalSlider_bj_T_2_valueChanged(int value);
    void on_verticalSlider_bj_T_3_valueChanged(int value);
    void on_verticalSlider_bj_T_4_valueChanged(int value);
    void on_verticalSlider_bj_T_5_valueChanged(int value);
    void on_verticalSlider_bj_T_6_valueChanged(int value);
    void on_verticalSlider_bj_T_7_valueChanged(int value);
    void on_verticalSlider_bj_T_8_valueChanged(int value);

    void on_verticalSlider_bj_TC_valueChanged(int value);
    void on_verticalSlider_bj_TC_2_valueChanged(int value);
    void on_verticalSlider_bj_TC_3_valueChanged(int value);
    void on_verticalSlider_bj_TC_4_valueChanged(int value);
    void on_verticalSlider_bj_TC_5_valueChanged(int value);
    void on_verticalSlider_bj_TC_6_valueChanged(int value);
    void on_verticalSlider_bj_TC_7_valueChanged(int value);
    void on_verticalSlider_bj_TC_8_valueChanged(int value);


    void on_battery_refresh_clicked();
    void on_strength_refresh_clicked();
    void on_button_readID_clicked();
    void on_button_setID_clicked();
    void on_qujian_set_clicked();
    void on_baoj_save_clicked();

    void on_baoj_save_pre_clicked();
    void on_button_save_set_para_clicked();
};

#endif // MAINWINDOW_H
