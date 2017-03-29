#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "General_code/General.h"
#include <QTextCodec>
#include "skinwidget.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
/*
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_item.h>
#include <qwt_legend_data.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_magnifier.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include "qwt.h"
#include "qwt_plot.h"
#include "qwt_scale_draw.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_curve.h"
#include "QTextCodec"
#include "qwt_plot_grid.h"
#include "qwt_symbol.h"
#include <QVBoxLayout>
*/

#define channel_num 16

//柱状图变量名定义


QVector<QString> labels(10);
double Pre[8][10];
QVector<double> values1(10);
QVector<double> values2(10);
QVector<double> values3(10);
QVector<double> values4(10);
QVector<double> values5(10);
QVector<double> values6(10);
QVector<double> values7(10);
QVector<double> values8(10);
double all[8]={0};
double sum[8][10]={{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1}};
float  zhu_wendu[8]={0};

QVector<QString>strength_labels(8);
QVector<double> strength_Pre(8);
QVector<double> strength_values(8);



double sum11=1;
double sum21=1;
double sum31=1;
double sum41=1;
double sum51=1;
double sum61=1;
double sum71=1;
double sum81=1;
double all11=0;
double all21=0;
double all31=0;
double all41=0;
double all51=0;
double all61=0;
double all71=0;
double all81=0;




QString str_min=0;
QString str_max=0;

QString str[10]={0};
QString str_fan[10]={0};
float val_min = 0;
float val_max = 0;
float more = 0;


//读取温度
float temperatrue_data_float_buf[channel_num];
float tempcha_data_float_buf[channel_num];

float downlimit_value_T=23;
float downlimit_value_TC=0.5;
int  baojing_T_value[channel_num];
int  baojing_TC_value[channel_num];

unsigned char device_ID=1;
unsigned char protocol_data_buf[8]={0x01,0x03,0x00,0x00,0x00,0x10,0x44,0x06};
QString protocol;
float battery_data_buf[channel_num];
unsigned char singal_strength_data_buf[channel_num];
unsigned char serial_data_buf[100];//串口数据缓冲区
bool status_buf[10]={0,0,0,0,0,0,0,0,0,0};
unsigned int device_sample_time=0;
unsigned char  sci_rev_full_flag=0;
unsigned char  debug_mode_flag=0;
float jinshui_wendu=0;
float liuliang=124;   //进出水流量（m3/s）
float refuhe_Cp=124;  //热负荷计算用的比热容    热负荷=流量*水温差*比热容
float reliuq_Cp=321;    //热流强度计算系数       热流强度=热负荷/冷却壁面积
//系统时间的获取与定义
#define TIMER_INTERVAL 2
/*
class TimeScaleDrawR: public QwtScaleDraw
{
public:
    TimeScaleDrawR(const QDateTime &base):
      baseTime(base)
      {
      }
      virtual QwtText label(double v) const
      {
          QDateTime upTime = baseTime.addSecs((int)v);
          return upTime.toString("hh:mm:ss");
      }
private:
    QDateTime baseTime;
};

*/
const unsigned short int wCRCTalbeAbs[]=
 {
    0x0000,0xCC01,0xD801,0x1400,0xF001,0x3C00,0x2800,0xE401,
    0xA001,0x6C00,0x7800,0xB401,0x5000,0x9C01,0x8801,0x4400,
 };
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);




    load_config_ini();
    initData();
    anniuyangshi();//初始化工具按钮状态样式
    //curveinstall();//曲线设置
    temc_Init();//温度初始化
    init_ui();//直方图对于那些不需要刷新的设置进行初始化，提高效率
    init_strength_ui();//直方图对于那些不需要刷新的设置进行初始化，提高效率
    biaoge_init();
    //q_init_ui();//曲线图进行初始化，提高效率

    on_actionOpen_triggered();//一打开就开启串口
    on_pushButton_2_clicked();//一打开就进入主画面

    QTimer *timer = new QTimer(this);//一秒定时器
    timer_sample = new QTimer(this);
    timer_serialport = new QTimer(this);
    QTimer *timer1 = new QTimer(this);
    //QTimer *timer_sample = new QTimer(this);//数据读取定时器
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    connect(timer_sample,SIGNAL(timeout()),this,SLOT(timer_sampleUpDate()));
    connect(timer_serialport,SIGNAL(timeout()),this,SLOT(timer_serialportUpDate()));
    connect(timer1,SIGNAL(timeout()),this,SLOT(timerUpDate1()));

    timer->start(device_sample_time);
    timer1->start(1000);
    timerUpDate();//为了一打开就显示。没有的话，则打开一秒后才显示
    timerUpDate1();
}

//解析函数
MainWindow::~MainWindow()
{
    if(myCom != NULL){
        if(myCom->isOpen()){
            myCom->close();
        }
        delete myCom;
    }
    delete ui;
}

void MainWindow::load_config_ini() // 加载配置信息
{
    QString a;
    QSettings settings("config.ini", QSettings::IniFormat);

    // 通讯接口信息加载
    //ui->portNameComboBox->set
    // 设备连接信息
    device_ID = settings.value("device_inf/id").toInt();
    device_sample_time = settings.value("device_inf/sample_time").toInt();
    ui->delayspinBox->setValue(device_sample_time);
    protocol= settings.value("device_inf/protocol").toString();

    // 报警信息加载
    baojing_T_value[0] = settings.value("alarm_inf/ch1_t").toInt();
    baojing_TC_value[0] = settings.value("alarm_inf/ch1_tc").toInt();
    ui->label_baoj_setT->setText(a.setNum(baojing_T_value[0])+" ℃");
    ui->label_baoj_setTC->setText(a.setNum(baojing_TC_value[0])+" ℃");

    baojing_T_value[1] = settings.value("alarm_inf/ch2_t").toInt();
    baojing_TC_value[1] = settings.value("alarm_inf/ch2_tc").toInt();
    ui->label_baoj_setT_2->setText(a.setNum(baojing_T_value[1])+" ℃");
    ui->label_baoj_setTC_2->setText(a.setNum(baojing_TC_value[1])+" ℃");

    baojing_T_value[2] = settings.value("alarm_inf/ch3_t").toInt();
    baojing_TC_value[2] = settings.value("alarm_inf/ch3_tc").toInt();
    ui->label_baoj_setT_3->setText(a.setNum(baojing_T_value[2])+" ℃");
    ui->label_baoj_setTC_3->setText(a.setNum(baojing_TC_value[2])+" ℃");

    baojing_T_value[3] = settings.value("alarm_inf/ch4_t").toInt();
    baojing_TC_value[3] = settings.value("alarm_inf/ch4_tc").toInt();
    ui->label_baoj_setT_4->setText(a.setNum(baojing_T_value[3])+" ℃");
    ui->label_baoj_setTC_4->setText(a.setNum(baojing_TC_value[3])+" ℃");

    baojing_T_value[4] = settings.value("alarm_inf/ch5_t").toInt();
    baojing_TC_value[4] = settings.value("alarm_inf/ch5_tc").toInt();
    ui->label_baoj_setT_5->setText(a.setNum(baojing_T_value[4])+" ℃");
    ui->label_baoj_setTC_5->setText(a.setNum(baojing_TC_value[4])+" ℃");

    baojing_T_value[5] = settings.value("alarm_inf/ch6_t").toInt();
    baojing_TC_value[5] = settings.value("alarm_inf/ch6_tc").toInt();
    ui->label_baoj_setT_6->setText(a.setNum(baojing_T_value[5])+" ℃");
    ui->label_baoj_setTC_6->setText(a.setNum(baojing_TC_value[5])+" ℃");

    baojing_T_value[6] = settings.value("alarm_inf/ch7_t").toInt();
    baojing_TC_value[6] = settings.value("alarm_inf/ch7_tc").toInt();
    ui->label_baoj_setT_7->setText(a.setNum(baojing_T_value[6])+" ℃");
    ui->label_baoj_setTC_7->setText(a.setNum(baojing_TC_value[6])+" ℃");

    baojing_T_value[7] = settings.value("alarm_inf/ch8_t").toInt();
    baojing_TC_value[7] = settings.value("alarm_inf/ch8_tc").toInt();
    ui->label_baoj_setT_8->setText(a.setNum(baojing_T_value[7])+" ℃");
    ui->label_baoj_setTC_8->setText(a.setNum(baojing_TC_value[7])+" ℃");

    // 柱状图区间信息加载
    str_min= settings.value("qujian_inf/qujian_min").toString();
    str_max= settings.value("qujian_inf/qujian_max").toString();
    val_min= settings.value("qujian_inf/qujian_min").toFloat();
    val_max= settings.value("qujian_inf/qujian_max").toFloat();
    ui->qujian_min->setText(str_min);
    ui->qujian_max->setText(str_max);

}
/*
//实时曲线
//定义8通道即右边小图标、两坐标轴的槽函数
void MainWindow::curveinstall()
{
    this->startTimer(1000);

    m_curve = new QwtPlotCurve("通道1");//创建一个新曲线，名为测试
    m_curve->setPen(QColor(Qt::red));
    //m_curve->setStyle(QwtPlotCurve::Lines);//设置曲线上是点还是线，默认是线，所以此行可不加
    m_curve->setRenderHint(QwtPlotItem::RenderAntialiased,true);//抗锯齿渲染
   // m_curve->setCurveAttribute(QwtPlotCurve::Fitted, true);//使曲线更光滑，不加这句曲线会很硬朗，有折点
    m_curve->attach(ui->qwtPlot);
    m_curve->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::red), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot->replot();

    m_curve1 = new QwtPlotCurve("通道2");//创建另一个新曲线，名为测试
    m_curve1->setPen(QColor(Qt::green));
    m_curve1->setRenderHint(QwtPlotItem::RenderAntialiased,true);//线条光滑化
    m_curve1->attach(ui->qwtPlot);
    m_curve1->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::green), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot->replot();

    m_curve2 = new QwtPlotCurve("通道3");//创建一个新曲线，名为测试
    m_curve2->setPen(QColor(Qt::blue));
    //m_curve->setStyle(QwtPlotCurve::Lines);//设置曲线上是点还是线，默认是线，所以此行可不加
    m_curve2->setRenderHint(QwtPlotItem::RenderAntialiased,true);//抗锯齿渲染
   // m_curve->setCurveAttribute(QwtPlotCurve::Fitted, true);//使曲线更光滑，不加这句曲线会很硬朗，有折点
    m_curve2->attach(ui->qwtPlot);
    m_curve2->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::blue), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot->replot();

    m_curve3 = new QwtPlotCurve("通道4");//创建一个新曲线，名为测试
    m_curve3->setPen(QColor(Qt::black));
    //m_curve->setStyle(QwtPlotCurve::Lines);//设置曲线上是点还是线，默认是线，所以此行可不加
    m_curve3->setRenderHint(QwtPlotItem::RenderAntialiased,true);//抗锯齿渲染
   // m_curve->setCurveAttribute(QwtPlotCurve::Fitted, true);//使曲线更光滑，不加这句曲线会很硬朗，有折点
    m_curve3->attach(ui->qwtPlot);
    m_curve3->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::black), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot->replot();

    m_curve4 = new QwtPlotCurve("通道5");//创建另一个新曲线，名为测试
    m_curve4->setPen(QColor(Qt::red));
    m_curve4->setRenderHint(QwtPlotItem::RenderAntialiased,true);//线条光滑化
    m_curve4->attach(ui->qwtPlot_2);
    m_curve4->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::red), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot_2->replot();

    m_curve5 = new QwtPlotCurve("通道6");//创建一个新曲线，名为测试
    m_curve5->setPen(QColor(Qt::green));
    //m_curve->setStyle(QwtPlotCurve::Lines);//设置曲线上是点还是线，默认是线，所以此行可不加
    m_curve5->setRenderHint(QwtPlotItem::RenderAntialiased,true);//抗锯齿渲染
   // m_curve->setCurveAttribute(QwtPlotCurve::Fitted, true);//使曲线更光滑，不加这句曲线会很硬朗，有折点
    m_curve5->attach(ui->qwtPlot_2);
    m_curve5->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::green), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot_2->replot();

    m_curve6 = new QwtPlotCurve("通道7");//创建一个新曲线，名为测试
    m_curve6->setPen(QColor(Qt::blue));
    //m_curve->setStyle(QwtPlotCurve::Lines);//设置曲线上是点还是线，默认是线，所以此行可不加
    m_curve6->setRenderHint(QwtPlotItem::RenderAntialiased,true);//抗锯齿渲染
   // m_curve->setCurveAttribute(QwtPlotCurve::Fitted, true);//使曲线更光滑，不加这句曲线会很硬朗，有折点
    m_curve6->attach(ui->qwtPlot_2);
    m_curve6->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::blue), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot_2->replot();

    m_curve7 = new QwtPlotCurve("通道8");//创建一个新曲线，名为测试
    m_curve7->setPen(QColor(Qt::black));
    //m_curve->setStyle(QwtPlotCurve::Lines);//设置曲线上是点还是线，默认是线，所以此行可不加
    m_curve7->setRenderHint(QwtPlotItem::RenderAntialiased,true);//抗锯齿渲染
   // m_curve->setCurveAttribute(QwtPlotCurve::Fitted, true);//使曲线更光滑，不加这句曲线会很硬朗，有折点
    m_curve7->attach(ui->qwtPlot_2);
    m_curve7->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::NoBrush,
    QPen(Qt::black), QSize(5, 5) ) );//定义一个QwtSymbol，来描述你点的一些参数，例如颜色，大小等
    ui->qwtPlot_2->replot();


    QwtLegend *legend = new QwtLegend;
    //图例可以选择,Checkable
    legend->setDefaultItemMode( QwtLegendData::Checkable );
    //pwtPlot中插入图例
    ui->qwtPlot->insertLegend(legend, QwtPlot::RightLegend );//把标号小图标放在右边
    //ui->qwtPlot->insertLegend(legend, QwtPlot::LeftLegend );//把标号小图标放在左边
    //连接槽，处理选择事件
    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
             SLOT( legendChecked( const QVariant &, bool ) ) );

    QwtLegend *legend1 = new QwtLegend;
    //图例可以选择,Checkable
    legend1->setDefaultItemMode( QwtLegendData::Checkable );
    //pwtPlot中插入图例
    ui->qwtPlot_2->insertLegend(legend1, QwtPlot::RightLegend);
    //连接槽，处理选择事件
    connect( legend1, SIGNAL( checked( const QVariant &, bool, int ) ),
             SLOT( legendChecked( const QVariant &, bool ) ) );


}
*/
/*
//实时曲线坐标轴初始化的设置
void MainWindow::q_init_ui()
{
    ui->qwtPlot->setTitle("实时温度曲线1-4");
    ui->qwtPlot_2->setTitle("实时温度曲线5-8");

    ui->qwtPlot->canvas()->setPalette(QPalette (QColor(Qt::white)));//曲线背景颜色
    ui->qwtPlot_2->canvas()->setPalette(QPalette (QColor(Qt::white)));

    QwtPlotZoomer* zoomer = new QwtPlotZoomer( ui->qwtPlot->canvas() );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::black ) );
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton );
    QwtPlotZoomer* zoomer1 = new QwtPlotZoomer( ui->qwtPlot_2->canvas() );
    zoomer1->setRubberBandPen( QColor( Qt::black ) );
    zoomer1->setTrackerPen( QColor( Qt::black ) );
    zoomer1->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier );
    zoomer1->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton );


    ui->qwtPlot->canvas()->setPalette(QPalette (QColor(Qt::darkCyan)));
    //曲线
    (new QwtPlotPanner(ui->qwtPlot->canvas()))->setMouseButton(Qt::RightButton);

    //y轴在放大的时候，坐标不变化
    (new QwtPlotMagnifier(ui->qwtPlot->canvas()))->setAxisEnabled(QwtPlot::yLeft,false);

    ui->qwtPlot_2->canvas()->setPalette(QPalette (QColor(Qt::darkCyan)));
    //曲线
    (new QwtPlotPanner(ui->qwtPlot_2->canvas()))->setMouseButton(Qt::RightButton);

    //y轴在放大的时候，坐标不变化
    (new QwtPlotMagnifier(ui->qwtPlot_2->canvas()))->setAxisEnabled(QwtPlot::yLeft,false);


    //一个选择器，十字线，以xBottom和yLeft坐标
    QwtPlotPicker * picker;
    picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                               ui->qwtPlot->canvas());
    picker->setStateMachine(new QwtPickerDragPointMachine());//拖拽点起作用
    picker->setRubberBandPen(QPen(QColor(Qt::white)));
    picker->setTrackerPen(QColor(Qt::yellow));

    QwtPlotPicker * picker1;
    picker1 = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                               QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                               ui->qwtPlot_2->canvas());
    picker1->setStateMachine(new QwtPickerDragPointMachine());//拖拽点起作用
    picker1->setRubberBandPen(QPen(QColor(Qt::white)));
    picker1->setTrackerPen(QColor(Qt::yellow));

    setAutoFillBackground(true);
    m_timeAreaWidth=120;//设置坐标间隔大小
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 80);
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "温度值/℃");
    m_baseTime = QDateTime::currentDateTime();
    ui->qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDrawR(m_baseTime));
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, 	m_timeAreaWidth);//设置x轴坐标刻度大小
    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom,"当前时间");
    ui->qwtPlot->setAxisLabelRotation(QwtPlot::xBottom, 0);//标签旋转50度
    ui->qwtPlot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft|Qt::AlignBottom);//x轴的数据是左对齐，下对齐
    ui->qwtPlot->plotLayout()->setAlignCanvasToScales(true);//设置对齐画布、坐标轴、刻度
    QwtPlotCanvas *canvas = new QwtPlotCanvas();//画布
    canvas->setBorderRadius(10);//画布四周边框角半径
    for ( int i = 0; i < m_timeAreaWidth; i++ )
        m_timeData[i] =-i;

    m_timeAreaWidth=120;//设置坐标间隔大小
    ui->qwtPlot_2->setAxisScale(QwtPlot::yLeft, 0, 80);
    ui->qwtPlot_2->setAxisTitle(QwtPlot::yLeft, "温度值/℃");
    m_baseTime = QDateTime::currentDateTime();
    ui->qwtPlot_2->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDrawR(m_baseTime));
    ui->qwtPlot_2->setAxisScale(QwtPlot::xBottom, 0, 	m_timeAreaWidth);//设置x轴坐标刻度大小
    ui->qwtPlot_2->setAxisTitle(QwtPlot::xBottom,"当前时间");
    ui->qwtPlot_2->setAxisLabelRotation(QwtPlot::xBottom, 0);//标签旋转50度
    ui->qwtPlot_2->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft|Qt::AlignBottom);//x轴的数据是左对齐，下对齐
    ui->qwtPlot_2->plotLayout()->setAlignCanvasToScales(true);//设置对齐画布、坐标轴、刻度
    QwtPlotCanvas *canvas1 = new QwtPlotCanvas();//画布
    canvas1->setBorderRadius(10);//画布四周边框角半径
    for ( int i = 0; i < m_timeAreaWidth; i++ )
        m_timeData[i] =-i;

    QwtPlotGrid *grid = new QwtPlotGrid;//网格
    grid->enableXMin(true);
    grid->setMajorPen(QPen(Qt::red, 0,  Qt::DotLine));//大格子
    grid->setMinorPen(QPen(Qt::white, 0 , Qt::DotLine));//大格子里的小格子
    grid->attach(ui->qwtPlot);

    QwtPlotGrid *grid1 = new QwtPlotGrid;//网格
    grid1->enableXMin(true);
    grid1->setMajorPen(QPen(Qt::red, 0, Qt::DotLine));//大格子
    grid1->setMinorPen(QPen(Qt::white, 0 , Qt::DotLine));//大格子里的小格子
    grid1->attach(ui->qwtPlot_2);
}
*/
//这个函数由两部分组成：（1）实时曲线x-y轴定义以及时间显示；（2）8通道报警指示灯显示
void MainWindow::timerEvent( QTimerEvent * )//定时器，
{
    /*
    //实时曲线x-y轴定义以及时间显示
   qint64 timeVal = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;
   qint64 timeVal1 = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;
   qint64 timeVal2 = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;
   qint64 timeVal3 = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;
   qint64 timeVal4 = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;
   qint64 timeVal5 = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;
   qint64 timeVal6 = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;
   qint64 timeVal7 = (QDateTime::currentDateTime().toMSecsSinceEpoch()-m_baseTime.toMSecsSinceEpoch())/1000;

   QPointF pt;
   QPointF pt1;
   QPointF pt2;
   QPointF pt3;
   QPointF pt4;
   QPointF pt5;
   QPointF pt6;
   QPointF pt7;

   //读取温度
   //float temperatrue_data_float_buf[16];
   //float tempcha_data_float_buf[16];

   pt.setX(timeVal);
   pt.setY(temperatrue_data_float_buf[0]);
   pt1.setX(timeVal1);
   pt1.setY(temperatrue_data_float_buf[1]);

   pt2.setX(timeVal1);
   pt2.setY(temperatrue_data_float_buf[2]);

   pt3.setX(timeVal1);  
   pt3.setY(temperatrue_data_float_buf[3]);


   pt4.setX(timeVal);
   pt4.setY(temperatrue_data_float_buf[4]);

   pt5.setX(timeVal1);
   pt5.setY(temperatrue_data_float_buf[5]);

   pt6.setX(timeVal1);
   pt6.setY(temperatrue_data_float_buf[6]);

   pt7.setX(timeVal1);
   pt7.setY(temperatrue_data_float_buf[7]);

   m_vp.append(pt);
   m_vp1.append(pt1);
   m_vp2.append(pt2);
   m_vp3.append(pt3);
   m_vp4.append(pt4);
   m_vp5.append(pt5);
   m_vp6.append(pt6);
   m_vp7.append(pt7);

   qint64 k = timeVal;
   qint64 t = timeVal1;
   qint64 g = timeVal2;
   qint64 h = timeVal3;
   qint64 r = timeVal4;
   qint64 d = timeVal5;
   qint64 s = timeVal6;
   qint64 m = timeVal7;

   for ( int j = 0; j < m_timeAreaWidth; j++ )
   {
       m_timeData[j] = k--;
   }
   for ( int i = 0; i < m_timeAreaWidth; i++ )
   {
       m_timeData[i] = t--;
   }
   for ( int j = 0; j < m_timeAreaWidth; j++ )
   {
       m_timeData[j] = g--;
   }
   for ( int j = 0; j < m_timeAreaWidth; j++ )
   {
       m_timeData[j] = h--;
   }

   for ( int i = 0; i < m_timeAreaWidth; i++ )
   {
       m_timeData[i] = r--;
   }
   for ( int j = 0; j < m_timeAreaWidth; j++ )
   {
       m_timeData[j] = d--;
   }
   for ( int j = 0; j < m_timeAreaWidth; j++ )
   {
       m_timeData[j] = s--;
   }
   for ( int j = 0; j < m_timeAreaWidth; j++ )
   {
       m_timeData[j] = m--;
   }
   ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve->setSamples(m_vp);
   ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve1->setSamples(m_vp1);
   ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve2->setSamples(m_vp2);
   ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve3->setSamples(m_vp3);
   ui->qwtPlot->replot();

   ui->qwtPlot_2->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve4->setSamples(m_vp4);
   ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve5->setSamples(m_vp5);
   ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve6->setSamples(m_vp6);
   ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 	m_timeData[m_timeAreaWidth - 1 ], m_timeData[0]);
   m_curve7->setSamples(m_vp7);
   ui->qwtPlot_2->replot();


*/
}
/*
//第一个坐标轴实时曲线的显示
void MainWindow::legendChecked( const QVariant &itemInfo, bool on )
{
    //获取曲线
    QwtPlotItem *plotItem = ui->qwtPlot->infoToItem( itemInfo );
    //根据图例选择状态，设置曲线隐藏还是显示
    if ( plotItem )
        plotItem->setVisible(on);
    //重要，下面这句没有不起作用
    ui->qwtPlot->replot();
}

//第二个坐标轴实时曲线的显示
void MainWindow::legendChecked1( const QVariant &itemInfo, bool on )
{
    //获取曲线
    QwtPlotItem *plotItem = ui->qwtPlot_2->infoToItem( itemInfo );
    //根据图例选择状态，设置曲线隐藏还是显示
    if ( plotItem )
        plotItem->setVisible(on);
    //重要，下面这句没有不起作用
    ui->qwtPlot_2->replot();
}
*/
void MainWindow::temc_Init()//温差初始化
{
    //ui->pushButton_1->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
    //ui->pushButton_45->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
    //ui->pushButton_49->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
    //ui->pushButton_22->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
    //ui->pushButton_19->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
    //ui->pushButton_28->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
    //ui->pushButton_55->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
    //ui->pushButton_56->setStyleSheet("font-size:20px;color:blue;font-weight:bold;background:yellow;fontstyle:italic");
}



void MainWindow::biaoge_init()
{
    //ui->tableWidget->horizontalHeaderItem(9)->setFont(QFont("wenquanyi",20,QFont::Bold));//表头哪一行的字体大小，并加粗
    ui->tableWidget->horizontalHeaderItem(0)->setFont(QFont("wenquanyi",10,10));//没加粗
    ui->tableWidget->horizontalHeaderItem(1)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeaderItem(2)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeaderItem(3)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeaderItem(4)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeaderItem(5)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeaderItem(6)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeaderItem(7)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeaderItem(8)->setFont(QFont("wenquanyi",10,10));
    ui->tableWidget->horizontalHeader()->setFixedHeight(35); //设置表头的高度
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}"); //设置表头背景色
    //ui->tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}"); //设置表头背景色
    ui->tableWidget->verticalHeader()->setVisible(false);//设置垂直头不可见,即左边序号不见,隐藏是因为最左上端那个格子不好设置
    QTableWidgetItem *item0_0, *item0_1,*item0_2, *item0_3;//设置4个左边标号即温度、温差、热负荷、热流强度
    item0_0 = new QTableWidgetItem;//创建一个QTableWidgetItem对象
    item0_1 = new QTableWidgetItem;
    item0_2 = new QTableWidgetItem;
    item0_3 = new QTableWidgetItem;
    item0_0->setTextAlignment(Qt::AlignCenter);//让设置的名称居中
    item0_1->setTextAlignment(Qt::AlignCenter);
    item0_2->setTextAlignment(Qt::AlignCenter);
    item0_3->setTextAlignment(Qt::AlignCenter);
    item0_0->setBackgroundColor(QColor(0,255,255));//设置名称的背景色
    item0_1->setBackgroundColor(QColor(0,255,255));
    item0_2->setBackgroundColor(QColor(0,255,255));
    item0_3->setBackgroundColor(QColor(0,255,255));
    item0_0->setText(QString("温度/℃"));//设置名称的名字
    item0_1->setText(QString("温差/℃"));
    item0_2->setText(QString("热负荷(kJ/m3)"));
    item0_3->setText(QString("热流强(kJ/h)"));
    ui->tableWidget->setItem(0, 0, item0_0);//设置名称对应的名字
    ui->tableWidget->setItem(1, 0, item0_1);
    ui->tableWidget->setItem(2, 0, item0_2);
    ui->tableWidget->setItem(3, 0, item0_3);
    ui->tableWidget->setColumnWidth(0,81);//设置第一列的列宽
    ui->tableWidget->setColumnWidth(1,81);//设置第一列的列宽
    ui->tableWidget->setColumnWidth(2,81);//设置第一列的列宽
    ui->tableWidget->setColumnWidth(3,81);//设置第一列的列宽
    ui->tableWidget->setColumnWidth(4,81);//设置第一列的列宽
    ui->tableWidget->setColumnWidth(5,81);//设置第一列的列宽
    ui->tableWidget->setColumnWidth(6,81);//设置第一列的列宽
    ui->tableWidget->setColumnWidth(7,81);//设置第一列的列宽
    ui->tableWidget->verticalHeader()->resizeSection(0,60);//设置行高
    ui->tableWidget->verticalHeader()->resizeSection(1,60);
    ui->tableWidget->verticalHeader()->resizeSection(2,60);
    ui->tableWidget->verticalHeader()->resizeSection(3,60);
    ui->tableWidget->setFont(QFont("wenquanyi",20,20));//字体大小

    item1_0 = new QTableWidgetItem;////创建一个QTableWidgetItem对象
    item1_1 = new QTableWidgetItem;
    item1_2 = new QTableWidgetItem;
    item1_3 = new QTableWidgetItem;
    item1_4 = new QTableWidgetItem;
    item1_5 = new QTableWidgetItem;
    item1_6 = new QTableWidgetItem;
    item1_7 = new QTableWidgetItem;
    item2_0 = new QTableWidgetItem;
    item2_1 = new QTableWidgetItem;
    item2_2 = new QTableWidgetItem;
    item2_3 = new QTableWidgetItem;
    item2_4 = new QTableWidgetItem;
    item2_5 = new QTableWidgetItem;
    item2_6 = new QTableWidgetItem;
    item2_7 = new QTableWidgetItem;
    item3_0 = new QTableWidgetItem;
    item3_1 = new QTableWidgetItem;
    item3_2 = new QTableWidgetItem;
    item3_3 = new QTableWidgetItem;
    item3_4 = new QTableWidgetItem;
    item3_5 = new QTableWidgetItem;
    item3_6 = new QTableWidgetItem;
    item3_7 = new QTableWidgetItem;
    item4_0 = new QTableWidgetItem;
    item4_1 = new QTableWidgetItem;
    item4_2 = new QTableWidgetItem;
    item4_3 = new QTableWidgetItem;
    item4_4 = new QTableWidgetItem;
    item4_5 = new QTableWidgetItem;
    item4_6 = new QTableWidgetItem;
    item4_7 = new QTableWidgetItem;

    item1_0->setTextAlignment(Qt::AlignCenter);//让设置的居中
    item1_1->setTextAlignment(Qt::AlignCenter);
    item1_2->setTextAlignment(Qt::AlignCenter);
    item1_3->setTextAlignment(Qt::AlignCenter);
    item1_4->setTextAlignment(Qt::AlignCenter);
    item1_5->setTextAlignment(Qt::AlignCenter);
    item1_6->setTextAlignment(Qt::AlignCenter);
    item1_7->setTextAlignment(Qt::AlignCenter);
    item2_0->setTextAlignment(Qt::AlignCenter);
    item2_1->setTextAlignment(Qt::AlignCenter);
    item2_2->setTextAlignment(Qt::AlignCenter);
    item2_3->setTextAlignment(Qt::AlignCenter);
    item2_4->setTextAlignment(Qt::AlignCenter);
    item2_5->setTextAlignment(Qt::AlignCenter);
    item2_6->setTextAlignment(Qt::AlignCenter);
    item2_7->setTextAlignment(Qt::AlignCenter);
    item3_0->setTextAlignment(Qt::AlignCenter);
    item3_1->setTextAlignment(Qt::AlignCenter);
    item3_2->setTextAlignment(Qt::AlignCenter);
    item3_3->setTextAlignment(Qt::AlignCenter);
    item3_4->setTextAlignment(Qt::AlignCenter);
    item3_5->setTextAlignment(Qt::AlignCenter);
    item3_6->setTextAlignment(Qt::AlignCenter);
    item3_7->setTextAlignment(Qt::AlignCenter);
    item4_0->setTextAlignment(Qt::AlignCenter);
    item4_1->setTextAlignment(Qt::AlignCenter);
    item4_2->setTextAlignment(Qt::AlignCenter);
    item4_3->setTextAlignment(Qt::AlignCenter);
    item4_4->setTextAlignment(Qt::AlignCenter);
    item4_5->setTextAlignment(Qt::AlignCenter);
    item4_6->setTextAlignment(Qt::AlignCenter);
    item4_7->setTextAlignment(Qt::AlignCenter);

    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);//第一列序号字体放中间
    QHeaderView *headerView = ui->tableWidget->horizontalHeader();
    headerView->setClickable(false);//不响应鼠标单击
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //很重要，可以消除表格最右边空隙即让表格填充宽度
    ui->tableWidget->verticalHeader()->setStretchLastSection(true);//设置充满表高度
    QTableWidgetItem *columnHeaderItem0 = ui->tableWidget->horizontalHeaderItem(0); //获得水平方向表头的Item对象
    columnHeaderItem0->setBackgroundColor(QColor(255,255,0)); //设置水平方向表头单元格背景颜色
    //columnHeaderItem0->setTextColor(QColor(200,111,30)); //设置文字颜色
}
//表格与柱状图的设置
void MainWindow::timerUpDate()
{
  //表格设置
   QDateTime time = QDateTime::currentDateTime();
   QString str = time.toString("yyyy-MM-dd hh:mm"); //设置系统时间显示格式
    //QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd"); //设置系统时间显示格式
    ui->label_38->setText(str); //在标签上显示时间
    //ui->label_38->setStyleSheet("font-size:22px;color:green;font-weight:bold;text-align:left;fontstyle:italic");


    //--------------------主画面-------------------------------------///
    QString abc;
    ui->lineEdit_TCH1->setText(abc.setNum(temperatrue_data_float_buf[0])+" ℃");
    ui->lineEdit_TCH2->setText(abc.setNum(temperatrue_data_float_buf[1])+" ℃");
    ui->lineEdit_TCH3->setText(abc.setNum(temperatrue_data_float_buf[2])+" ℃");
    ui->lineEdit_TCH4->setText(abc.setNum(temperatrue_data_float_buf[3])+" ℃");
    ui->lineEdit_TCH5->setText(abc.setNum(temperatrue_data_float_buf[4])+" ℃");
    ui->lineEdit_TCH6->setText(abc.setNum(temperatrue_data_float_buf[5])+" ℃");
    ui->lineEdit_TCH7->setText(abc.setNum(temperatrue_data_float_buf[6])+" ℃");
    ui->lineEdit_TCH8->setText(abc.setNum(temperatrue_data_float_buf[7])+" ℃");
    //8通道报警指示灯显示
    if((temperatrue_data_float_buf[0]>=baojing_T_value[0])||(temperatrue_data_float_buf[0]<=downlimit_value_T)) ui->pushButton_10->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_10->setStyleSheet("background-color:rgba(0, 255, 255, 255);");
        if((temperatrue_data_float_buf[1]>=baojing_T_value[1])||(temperatrue_data_float_buf[1]<=downlimit_value_T))ui->pushButton_11->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_11->setStyleSheet("background-color:rgba(0, 255, 255, 255);");
        if((temperatrue_data_float_buf[2]>=baojing_T_value[2])||(temperatrue_data_float_buf[2]<=downlimit_value_T))ui->pushButton_12->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_12->setStyleSheet("background-color:rgba(0, 255, 255, 255);");
        if((temperatrue_data_float_buf[3]>=baojing_T_value[3])||(temperatrue_data_float_buf[3]<=downlimit_value_T))ui->pushButton_13->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_13->setStyleSheet("background-color:rgba(0, 255, 255, 255);");
        if((temperatrue_data_float_buf[4]>=baojing_T_value[4])||(temperatrue_data_float_buf[4]<=downlimit_value_T))ui->pushButton_14->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_14->setStyleSheet("background-color:rgba(0, 255, 255, 255);");
        if((temperatrue_data_float_buf[5]>=baojing_T_value[5])||(temperatrue_data_float_buf[5]<=downlimit_value_T))ui->pushButton_15->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_15->setStyleSheet("background-color:rgba(0, 255, 255, 255);");
        if((temperatrue_data_float_buf[6]>=baojing_T_value[6])||(temperatrue_data_float_buf[6]<=downlimit_value_T))ui->pushButton_16->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_16->setStyleSheet("background-color:rgba(0, 255, 255, 255);");
        if((temperatrue_data_float_buf[7]>=baojing_T_value[7])||(temperatrue_data_float_buf[7]<=downlimit_value_T))ui->pushButton_17->setStyleSheet("background-color:rgba(255, 0, 0, 255);");
        else ui->pushButton_17->setStyleSheet("background-color:rgba(0, 255, 255, 255);");

    //----------------------------------------------------------------//




    //----------------------网络通讯状态------------------------------------///
    QPixmap a;
    if(temperatrue_data_float_buf[0]>0){a.load(":/res/image/jiedian2.png");ui->jiedian->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian->setPixmap(a);}
    if(temperatrue_data_float_buf[1]>0){a.load(":/res/image/jiedian2.png");ui->jiedian_2->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian_2->setPixmap(a);}
    if(temperatrue_data_float_buf[2]>0){a.load(":/res/image/jiedian2.png");ui->jiedian_3->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian_3->setPixmap(a);}
    if(temperatrue_data_float_buf[3]>0){a.load(":/res/image/jiedian2.png");ui->jiedian_4->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian_4->setPixmap(a);}
    if(temperatrue_data_float_buf[4]>0){a.load(":/res/image/jiedian2.png");ui->jiedian_5->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian_5->setPixmap(a);}
    if(temperatrue_data_float_buf[5]>0){a.load(":/res/image/jiedian2.png");ui->jiedian_6->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian_6->setPixmap(a);}
    if(temperatrue_data_float_buf[6]>0){a.load(":/res/image/jiedian2.png");ui->jiedian_7->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian_7->setPixmap(a);}
    if(temperatrue_data_float_buf[7]>0){a.load(":/res/image/jiedian2.png");ui->jiedian_8->setPixmap(a);}
    else{a.load(":/res/image/jiedian.png"); ui->jiedian_8->setPixmap(a);}
    //----------------------------------------------------------------//




    //----------------------采集终端监控------------------------------------///

    //----------------------------------------------------------------//



    //----------------------------水温差及热负荷------------------------//

    item1_0->setText(QString("%1").arg(temperatrue_data_float_buf[0]));//获取温度
    item1_1->setText(QString("%1").arg(temperatrue_data_float_buf[1]));
    item1_2->setText(QString("%1").arg(temperatrue_data_float_buf[2]));
    item1_3->setText(QString("%1").arg(temperatrue_data_float_buf[3]));
    item1_4->setText(QString("%1").arg(temperatrue_data_float_buf[4]));
    item1_5->setText(QString("%1").arg(temperatrue_data_float_buf[5]));
    item1_6->setText(QString("%1").arg(temperatrue_data_float_buf[6]));
    item1_7->setText(QString("%1").arg(temperatrue_data_float_buf[7]));


    jinshui_wendu=temperatrue_data_float_buf[1]-1.9;
    for(int i=0;i<8;i++)
    {
      if(temperatrue_data_float_buf[i]>0)tempcha_data_float_buf[i]=temperatrue_data_float_buf[i]-jinshui_wendu;
    }

    item2_0->setText(QString("%1").arg(tempcha_data_float_buf[0]));//获取温差
    item2_1->setText(QString("%1").arg(tempcha_data_float_buf[1]));
    item2_2->setText(QString("%1").arg(tempcha_data_float_buf[2]));
    item2_3->setText(QString("%1").arg(tempcha_data_float_buf[3]));
    item2_4->setText(QString("%1").arg(tempcha_data_float_buf[4]));
    item2_5->setText(QString("%1").arg(tempcha_data_float_buf[5]));
    item2_6->setText(QString("%1").arg(tempcha_data_float_buf[6]));
    item2_7->setText(QString("%1").arg(tempcha_data_float_buf[7]));

    item3_0->setText(QString("%1").arg(tempcha_data_float_buf[0]*liuliang*refuhe_Cp));//获热负荷
    item3_1->setText(QString("%1").arg(tempcha_data_float_buf[1]*liuliang*refuhe_Cp));
    item3_2->setText(QString("%1").arg(tempcha_data_float_buf[2]*liuliang*refuhe_Cp));
    item3_3->setText(QString("%1").arg(tempcha_data_float_buf[3]*liuliang*refuhe_Cp));
    item3_4->setText(QString("%1").arg(tempcha_data_float_buf[4]*liuliang*refuhe_Cp));
    item3_5->setText(QString("%1").arg(tempcha_data_float_buf[5]*liuliang*refuhe_Cp));
    item3_6->setText(QString("%1").arg(tempcha_data_float_buf[6]*liuliang*refuhe_Cp));
    item3_7->setText(QString("%1").arg(tempcha_data_float_buf[7]*liuliang*refuhe_Cp));

    item4_0->setText(QString("%1").arg(tempcha_data_float_buf[0]*liuliang*reliuq_Cp));//获取热流强度
    item4_1->setText(QString("%1").arg(tempcha_data_float_buf[1]*liuliang*reliuq_Cp));
    item4_2->setText(QString("%1").arg(tempcha_data_float_buf[2]*liuliang*reliuq_Cp));
    item4_3->setText(QString("%1").arg(tempcha_data_float_buf[3]*liuliang*reliuq_Cp));
    item4_4->setText(QString("%1").arg(tempcha_data_float_buf[4]*liuliang*reliuq_Cp));
    item4_5->setText(QString("%1").arg(tempcha_data_float_buf[5]*liuliang*reliuq_Cp));
    item4_6->setText(QString("%1").arg(tempcha_data_float_buf[6]*liuliang*reliuq_Cp));
    item4_7->setText(QString("%1").arg(tempcha_data_float_buf[7]*liuliang*reliuq_Cp));


    ui->tableWidget->setItem(0, 1, item1_0);//温度与温差写入指定表格
    ui->tableWidget->setItem(0, 2, item1_1);
    ui->tableWidget->setItem(0, 3, item1_2);
    ui->tableWidget->setItem(0, 4, item1_3);
    ui->tableWidget->setItem(0, 5, item1_4);
    ui->tableWidget->setItem(0, 6, item1_5);
    ui->tableWidget->setItem(0, 7, item1_6);
    ui->tableWidget->setItem(0, 8, item1_7);

    ui->tableWidget->setItem(1, 1, item2_0);
    ui->tableWidget->setItem(1, 2, item2_1);
    ui->tableWidget->setItem(1, 3, item2_2);
    ui->tableWidget->setItem(1, 4, item2_3);
    ui->tableWidget->setItem(1, 5, item2_4);
    ui->tableWidget->setItem(1, 6, item2_5);
    ui->tableWidget->setItem(1, 7, item2_6);
    ui->tableWidget->setItem(1, 8, item2_7);

    ui->tableWidget->setItem(2, 1, item3_0);
    ui->tableWidget->setItem(2, 2, item3_1);
    ui->tableWidget->setItem(2, 3, item3_2);
    ui->tableWidget->setItem(2, 4, item3_3);
    ui->tableWidget->setItem(2, 5, item3_4);
    ui->tableWidget->setItem(2, 6, item3_5);
    ui->tableWidget->setItem(2, 7, item3_6);
    ui->tableWidget->setItem(2, 8, item3_7);

    ui->tableWidget->setItem(3, 1, item4_0);
    ui->tableWidget->setItem(3, 2, item4_1);
    ui->tableWidget->setItem(3, 3, item4_2);
    ui->tableWidget->setItem(3, 4, item4_3);
    ui->tableWidget->setItem(3, 5, item4_4);
    ui->tableWidget->setItem(3, 6, item4_5);
    ui->tableWidget->setItem(3, 7, item4_6);
    ui->tableWidget->setItem(3, 8, item4_7);


    if(tempcha_data_float_buf[0]>=5)//设置大于温差图标改变
    {
        item2_0->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_0->setBackgroundColor(QColor(170,170,255));
    }
    if(tempcha_data_float_buf[1]>=5)
    {
        item2_1->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_1->setBackgroundColor(QColor(170,170,255));
    }
    if(tempcha_data_float_buf[2]>=5)
    {
        item2_2->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_2->setBackgroundColor(QColor(170,170,255));
    }
    if(tempcha_data_float_buf[3]>=5)
    {
        item2_3->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_3->setBackgroundColor(QColor(170,170,255));
    }
    if(tempcha_data_float_buf[4]>=5)
    {
        item2_4->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_4->setBackgroundColor(QColor(170,170,255));
    }
    if(tempcha_data_float_buf[5]>=5)
    {
        item2_5->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_5->setBackgroundColor(QColor(170,170,255));
    }
    if(tempcha_data_float_buf[6]>=5)
    {
        item2_6->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_6->setBackgroundColor(QColor(170,170,255));
    }
    if(tempcha_data_float_buf[7]>=5)
    {
        item2_7->setBackgroundColor(QColor(255,0,0));
    }
    else
    {
        item2_7->setBackgroundColor(QColor(170,170,255));
    }

    //QTableWidgetItem *item = new QTableWidgetItem();//设置表格内颜色
    //item->setBackgroundColor(QColor(255,255,0));
    //item->setTextColor(QColor(200,111,100));
    //item->setFont(QFont("Helvetica"));
    //ui->tableWidget->setItem(0,3,item);

    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);//第一列序号字体放中间
    QHeaderView *headerView = ui->tableWidget->horizontalHeader();
    headerView->setClickable(false);//不响应鼠标单击
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //很重要，可以消除表格最右边空隙即让表格填充宽度
    ui->tableWidget->verticalHeader()->setStretchLastSection(true);//设置充满表高度
    QTableWidgetItem *columnHeaderItem0 = ui->tableWidget->horizontalHeaderItem(0); //获得水平方向表头的Item对象
    columnHeaderItem0->setBackgroundColor(QColor(255,255,0)); //设置水平方向表头单元格背景颜色
    //columnHeaderItem0->setTextColor(QColor(200,111,30)); //设置文字颜色


    //---------------------温度柱状图-------------------------
       this->ui->widget->clearPlottables();//清除之前保留在坐标轴的数据图形
       this->ui->widget_2->clearPlottables();
       this->ui->widget_3->clearPlottables();
       this->ui->widget_4->clearPlottables();
       this->ui->widget_5->clearPlottables();
       this->ui->widget_6->clearPlottables();
       this->ui->widget_7->clearPlottables();
       this->ui->widget_8->clearPlottables();
       for(int i=0;i<8;i++)zhu_wendu[i]=temperatrue_data_float_buf[i];
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[0])>=((more/10)*i+val_min))&&((zhu_wendu[0])<((more/10)*i+val_min+(more/10))))
                   sum[0][i]=sum[0][i]+1;
               Pre[0][i]=sum[0][i];
           }
           all[0]=Pre[0][0]+Pre[0][1]+Pre[0][2]+Pre[0][3]+Pre[0][4]+Pre[0][5]+Pre[0][6]+Pre[0][7]+Pre[0][8]+Pre[0][9];
           for(int i=0;i<10;i++)
           {
               values1[i]=(Pre[0][i]*100)/(all[0]);
           }
        //zhu_wendu[1]=rand()%100;
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[1])>=((more/10)*i+val_min))&&((zhu_wendu[1])<((more/10)*i+val_min+(more/10))))
                   sum[1][i]=sum[1][i]+1;
               Pre[1][i]=sum[1][i];
           }
           all[1]=Pre[1][0]+Pre[1][1]+Pre[1][2]+Pre[1][3]+Pre[1][4]+Pre[1][5]+Pre[1][6]+Pre[1][7]+Pre[1][8]+Pre[1][9];
           for(int i=0;i<10;i++)
           {
               values2[i]=(Pre[1][i]*100)/(all[1]);
           }

        //zhu_wendu[2]=rand()%100;
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[2])>=((more/10)*i+val_min))&&((zhu_wendu[2])<((more/10)*i+val_min+(more/10))))
                   sum[2][i]=sum[2][i]+1;
               Pre[2][i]=sum[2][i];
           }
           all[2]=Pre[2][0]+Pre[2][1]+Pre[2][2]+Pre[2][3]+Pre[2][4]+Pre[2][5]+Pre[2][6]+Pre[2][7]+Pre[2][8]+Pre[2][9];
           for(int i=0;i<10;i++)
           {
               values3[i]=(Pre[2][i]*100)/(all[2]);
           }

        //zhu_wendu[3]=rand()%100;
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[3])>=((more/10)*i+val_min))&&((zhu_wendu[3])<((more/10)*i+val_min+(more/10))))
                   sum[3][i]=sum[3][i]+1;
               Pre[3][i]=sum[3][i];
           }
           all[3]=Pre[3][0]+Pre[3][1]+Pre[3][2]+Pre[3][3]+Pre[3][4]+Pre[3][5]+Pre[3][6]+Pre[3][7]+Pre[3][8]+Pre[3][9];
           for(int i=0;i<10;i++)
           {
               values4[i]=(Pre[3][i]*100)/(all[3]);
           }

       //zhu_wendu[4]=rand()%100;
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[4])>=((more/10)*i+val_min))&&((zhu_wendu[4])<((more/10)*i+val_min+(more/10))))
                   sum[4][i]=sum[4][i]+1;
               Pre[4][i]=sum[4][i];
           }
           all[4]=Pre[4][0]+Pre[4][1]+Pre[4][2]+Pre[4][3]+Pre[4][4]+Pre[4][5]+Pre[4][6]+Pre[4][7]+Pre[4][8]+Pre[4][9];
           for(int i=0;i<10;i++)
           {
               values5[i]=(Pre[4][i]*100)/(all[4]);
           }

       //zhu_wendu[5]=rand()%100;
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[5])>=((more/10)*i+val_min))&&((zhu_wendu[5])<((more/10)*i+val_min+(more/10))))
                   sum[5][i]=sum[5][i]+1;
               Pre[5][i]=sum[5][i];
           }
           all[5]=Pre[5][0]+Pre[5][1]+Pre[5][2]+Pre[5][3]+Pre[5][4]+Pre[5][5]+Pre[5][6]+Pre[5][7]+Pre[5][8]+Pre[5][9];
           for(int i=0;i<10;i++)
           {
               values6[i]=(Pre[5][i]*100)/(all[5]);
           }

       //zhu_wendu[6]=rand()%100;
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[6])>=((more/10)*i+val_min))&&((zhu_wendu[6])<((more/10)*i+val_min+(more/10))))
                   sum[6][i]=sum[6][i]+1;
               Pre[6][i]=sum[6][i];
           }
           all[6]=Pre[6][0]+Pre[6][1]+Pre[6][2]+Pre[6][3]+Pre[6][4]+Pre[6][5]+Pre[6][6]+Pre[6][7]+Pre[6][8]+Pre[6][9];
           for(int i=0;i<10;i++)
           {
               values7[i]=(Pre[6][i]*100)/(all[6]);
           }

       //zhu_wendu[7]=rand()%100;
           for(int i=0;i<10;i++)
           {
               if(((zhu_wendu[7])>=((more/10)*i+val_min))&&((zhu_wendu[7])<((more/10)*i+val_min+(more/10))))
                   sum[7][i]=sum[7][i]+1;
               Pre[7][i]=sum[7][i];
           }
           all[7]=Pre[7][0]+Pre[7][1]+Pre[7][2]+Pre[7][3]+Pre[7][4]+Pre[7][5]+Pre[7][6]+Pre[7][7]+Pre[7][8]+Pre[7][9];
           for(int i=0;i<10;i++)
           {
               values8[i]=(Pre[7][i]*100)/(all[7]);
           }


       QCPBars* bars=new QCPBars(this->ui->widget->xAxis,this->ui->widget->yAxis);
       QVector<double> index(10);
       for(int i=0;i<10;++i)
          index[i]=i+0.39;
       bars->setData(index,values1);

       QCPBars* bars2=new QCPBars(this->ui->widget_2->xAxis,this->ui->widget_2->yAxis);
       //QVector<double> index2(10);
       //for(int i=0;i<10;++i)
          //index2[i]=i+0.39;
       bars2->setData(index,values2);

       QCPBars* bars3=new QCPBars(this->ui->widget_3->xAxis,this->ui->widget_3->yAxis);
       bars3->setData(index,values3);
       QCPBars* bars4=new QCPBars(this->ui->widget_4->xAxis,this->ui->widget_4->yAxis);
       bars4->setData(index,values4);
       QCPBars* bars5=new QCPBars(this->ui->widget_5->xAxis,this->ui->widget_5->yAxis);
       bars5->setData(index,values5);
       QCPBars* bars6=new QCPBars(this->ui->widget_6->xAxis,this->ui->widget_6->yAxis);
       bars6->setData(index,values6);
       QCPBars* bars7=new QCPBars(this->ui->widget_7->xAxis,this->ui->widget_7->yAxis);
       bars7->setData(index,values7);
       QCPBars* bars8=new QCPBars(this->ui->widget_8->xAxis,this->ui->widget_8->yAxis);
       bars8->setData(index,values8);

       this->ui->widget->addPlottable(bars);
       double wid=this->ui->widget->xAxis->range().upper-this->ui->widget->xAxis->range().lower;
       double cl=bars->width()+(1.0*wid-bars->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor;
       for(int i=0;i<10;++i)
       {
           coor.append(this->ui->widget->xAxis->range().lower+i*cl+bars->width()/2);

       }
       this->ui->widget->xAxis->setTickVector(coor);//建立x-y轴坐标的
       this->ui->widget->xAxis->setTickVectorLabels(labels);

       this->ui->widget_2->addPlottable(bars2);
       double wid2=this->ui->widget_2->xAxis->range().upper-this->ui->widget_2->xAxis->range().lower;
       double cl2=bars2->width()+(1.0*wid2-bars2->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor2;
       for(int i=0;i<10;++i)
       {
           coor2.append(this->ui->widget_2->xAxis->range().lower+i*cl2+bars2->width()/2);

       }
       this->ui->widget_2->xAxis->setTickVector(coor2);
       this->ui->widget_2->xAxis->setTickVectorLabels(labels);

       this->ui->widget_3->addPlottable(bars3);
       double wid3=this->ui->widget_3->xAxis->range().upper-this->ui->widget_3->xAxis->range().lower;
       double cl3=bars3->width()+(1.0*wid3-bars3->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor3;
       for(int i=0;i<10;++i)
       {
           coor3.append(this->ui->widget_3->xAxis->range().lower+i*cl3+bars3->width()/2);

       }
       this->ui->widget_3->xAxis->setTickVector(coor3);
       this->ui->widget_3->xAxis->setTickVectorLabels(labels);

       this->ui->widget_4->addPlottable(bars4);
       double wid4=this->ui->widget_4->xAxis->range().upper-this->ui->widget_4->xAxis->range().lower;
       double cl4=bars4->width()+(1.0*wid4-bars4->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor4;
       for(int i=0;i<10;++i)
       {
           coor4.append(this->ui->widget_4->xAxis->range().lower+i*cl4+bars4->width()/2);

       }
       this->ui->widget_4->xAxis->setTickVector(coor4);
       this->ui->widget_4->xAxis->setTickVectorLabels(labels);

       this->ui->widget_5->addPlottable(bars5);
       double wid5=this->ui->widget_5->xAxis->range().upper-this->ui->widget_5->xAxis->range().lower;
       double cl5=bars5->width()+(1.0*wid5-bars5->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor5;
       for(int i=0;i<10;++i)
       {
           coor5.append(this->ui->widget_5->xAxis->range().lower+i*cl5+bars5->width()/2);

       }
       this->ui->widget_5->xAxis->setTickVector(coor5);
       this->ui->widget_5->xAxis->setTickVectorLabels(labels);

       this->ui->widget_6->addPlottable(bars6);
       double wid6=this->ui->widget_6->xAxis->range().upper-this->ui->widget_6->xAxis->range().lower;
       double cl6=bars6->width()+(1.0*wid6-bars6->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor6;
       for(int i=0;i<10;++i)
       {
           coor6.append(this->ui->widget_6->xAxis->range().lower+i*cl6+bars6->width()/2);

       }
       this->ui->widget_6->xAxis->setTickVector(coor6);
       this->ui->widget_6->xAxis->setTickVectorLabels(labels);

       this->ui->widget_7->addPlottable(bars7);
       double wid7=this->ui->widget_7->xAxis->range().upper-this->ui->widget_7->xAxis->range().lower;
       double cl7=bars7->width()+(1.0*wid7-bars7->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor7;
       for(int i=0;i<10;++i)
       {
           coor7.append(this->ui->widget_7->xAxis->range().lower+i*cl7+bars7->width()/2);

       }
       this->ui->widget_7->xAxis->setTickVector(coor7);
       this->ui->widget_7->xAxis->setTickVectorLabels(labels);

       this->ui->widget_8->addPlottable(bars8);
       double wid8=this->ui->widget_8->xAxis->range().upper-this->ui->widget_8->xAxis->range().lower;
       double cl8=bars8->width()+(1.0*wid8-bars8->width()*10)/10;//这句定义x坐标下标示的个数
       QVector<double> coor8;
       for(int i=0;i<10;++i)
       {
           coor8.append(this->ui->widget_8->xAxis->range().lower+i*cl8+bars8->width()/2);

       }
       this->ui->widget_8->xAxis->setTickVector(coor8);
       this->ui->widget_8->xAxis->setTickVectorLabels(labels);

       bars->setBrush(QColor(238,0,0,200));//柱添加颜色
       this->ui->widget->replot();
       bars2->setBrush(QColor(238,0,0,200));
       this->ui->widget_2->replot();
       bars3->setBrush(QColor(238,0,0,200));
       this->ui->widget_3->replot();
       bars4->setBrush(QColor(238,0,0,200));
       this->ui->widget_4->replot();
       bars5->setBrush(QColor(238,0,0,200));
       this->ui->widget_5->replot();
       bars6->setBrush(QColor(238,0,0,200));
       this->ui->widget_6->replot();
       bars7->setBrush(QColor(238,0,0,200));
       this->ui->widget_7->replot();
       bars8->setBrush(QColor(238,0,0,200));
       this->ui->widget_8->replot();
}

//柱状图的初始化
void MainWindow::init_ui()
{
    //QString bye[10]={"[40-41]","[41-42]","[42-43]","[43-44]","[44-45]","[45-46]","[46-47]","[47-48]","[48-49]","[49-50]"};
    //for(int i=0;i<10;i++)labels[i]=bye[i];
    this->ui->widget->yAxis->setLabel("百分比/%");
    this->ui->widget->xAxis->setLabel("温度范围/°C");
    this->ui->widget_2->yAxis->setLabel("百分比/%");
    this->ui->widget_2->xAxis->setLabel("温度范围/°C");
    this->ui->widget_3->yAxis->setLabel("百分比/%");
    this->ui->widget_3->xAxis->setLabel("温度范围/°C");
    this->ui->widget_4->yAxis->setLabel("百分比/%");
    this->ui->widget_4->xAxis->setLabel("温度范围/°C");
    this->ui->widget_5->yAxis->setLabel("百分比/%");
    this->ui->widget_5->xAxis->setLabel("温度范围/°C");
    this->ui->widget_6->yAxis->setLabel("百分比/%");
    this->ui->widget_6->xAxis->setLabel("温度范围/°C");
    this->ui->widget_7->yAxis->setLabel("百分比/%");
    this->ui->widget_7->xAxis->setLabel("温度范围/°C");
    this->ui->widget_8->yAxis->setLabel("百分比/%");
    this->ui->widget_8->xAxis->setLabel("温度范围/°C");

    //添加完了绘制的柱状图，接下来添加标签，要想完全自己定义标签，需要先执行以下代码关闭默认的底部标签自动生成
    this->ui->widget->xAxis->setAutoTicks(false);//以下这三句为关闭默认坐标取值
    this->ui->widget->xAxis->setAutoTickLabels(false);
    this->ui->widget->xAxis->setAutoTickStep(false);

    this->ui->widget_2->xAxis->setAutoTicks(false);//以下这三句为关闭默认坐标取值
    this->ui->widget_2->xAxis->setAutoTickLabels(false);
    this->ui->widget_2->xAxis->setAutoTickStep(false);

    this->ui->widget_3->xAxis->setAutoTicks(false);
    this->ui->widget_3->xAxis->setAutoTickLabels(false);
    this->ui->widget_3->xAxis->setAutoTickStep(false);

    this->ui->widget_4->xAxis->setAutoTicks(false);
    this->ui->widget_4->xAxis->setAutoTickLabels(false);
    this->ui->widget_4->xAxis->setAutoTickStep(false);

    this->ui->widget_5->xAxis->setAutoTicks(false);
    this->ui->widget_5->xAxis->setAutoTickLabels(false);
    this->ui->widget_5->xAxis->setAutoTickStep(false);

    this->ui->widget_6->xAxis->setAutoTicks(false);
    this->ui->widget_6->xAxis->setAutoTickLabels(false);
    this->ui->widget_6->xAxis->setAutoTickStep(false);

    this->ui->widget_7->xAxis->setAutoTicks(false);
    this->ui->widget_7->xAxis->setAutoTickLabels(false);
    this->ui->widget_7->xAxis->setAutoTickStep(false);

    this->ui->widget_8->xAxis->setAutoTicks(false);
    this->ui->widget_8->xAxis->setAutoTickLabels(false);
    this->ui->widget_8->xAxis->setAutoTickStep(false);
    //设置坐标轴显示范围,否则我们只能看到默认的范围
    this->ui->widget->xAxis->setRange(0,10);
    this->ui->widget->yAxis->setRange(0,100);

    this->ui->widget_2->xAxis->setRange(0,10);
    this->ui->widget_2->yAxis->setRange(0,100);

    this->ui->widget_3->xAxis->setRange(0,10);
    this->ui->widget_3->yAxis->setRange(0,100);

    this->ui->widget_4->xAxis->setRange(0,10);
    this->ui->widget_4->yAxis->setRange(0,100);

    this->ui->widget_5->xAxis->setRange(0,10);
    this->ui->widget_5->yAxis->setRange(0,100);

    this->ui->widget_6->xAxis->setRange(0,10);
    this->ui->widget_6->yAxis->setRange(0,100);

    this->ui->widget_7->xAxis->setRange(0,10);
    this->ui->widget_7->yAxis->setRange(0,100);

    this->ui->widget_8->xAxis->setRange(0,10);
    this->ui->widget_8->yAxis->setRange(0,100);
    //this->ui->widget->rescaleAxes();//它会自动设置最合适的显示范围，非常简便。
    //this->ui->widget->setAlignment(Qt::AlignHCenter);
    this->ui->widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //设置坐标背景色
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(200, 200, 200));
    //axisRectGradient.setColorAt(1, QColor(130, 130, 130));
    this->ui->widget->axisRect()->setBackground(axisRectGradient);
    this->ui->widget_2->axisRect()->setBackground(axisRectGradient);
    this->ui->widget_3->axisRect()->setBackground(axisRectGradient);
    this->ui->widget_4->axisRect()->setBackground(axisRectGradient);
    this->ui->widget_5->axisRect()->setBackground(axisRectGradient);
    this->ui->widget_6->axisRect()->setBackground(axisRectGradient);
    this->ui->widget_7->axisRect()->setBackground(axisRectGradient);
    this->ui->widget_8->axisRect()->setBackground(axisRectGradient);
    //设置画布背景色
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(200, 200, 200));
    //plotGradient.setColorAt(1, QColor(200, 200, 200));
    this->ui->widget->setBackground(plotGradient);
    this->ui->widget_2->setBackground(plotGradient);
    this->ui->widget_3->setBackground(plotGradient);
    this->ui->widget_4->setBackground(plotGradient);
    this->ui->widget_5->setBackground(plotGradient);
    this->ui->widget_6->setBackground(plotGradient);
    this->ui->widget_7->setBackground(plotGradient);
    this->ui->widget_8->setBackground(plotGradient);


   more = (val_max-val_min);
   if(val_min<=val_max)
   {
      //float more = (val_max-val_min);
      //QString str = QString("%1").arg(more);//将float类型转化为QString类型
      //ui->qujian_max->setText(str);//显示获取的值
      for(int i=0;i<10;i++)
         {
          str[i] = QString("%1").arg((more/10)*i+val_min);//使显示左边值
          str_fan[i] = QString("%1").arg((more/10)*i+val_min+(more/10));//使显示右边值
          labels[i] = QString("[")+(str[i])+QString("-")+(str_fan[i])+QString("]");//使显示的形式为[3-3.1]
          }
   }
}

void MainWindow::timerUpDate1()
{
    setupRealtimecurve(ui->customPlot);
    setupRealtimecurve2(ui->customPlot_2);
    setupRealtimecurve3(ui->customPlot_3);
    setupRealtimecurve4(ui->customPlot_4);
    setupRealtimecurve5(ui->customPlot_5);
    setupRealtimecurve6(ui->customPlot_6);
    setupRealtimecurve7(ui->customPlot_7);
    setupRealtimecurve8(ui->customPlot_8);
}
int trend_min=10;
int trend_max=50;
void MainWindow::setupRealtimecurve(QCustomPlot *customPlot)
{

    double Count=1500;
    //double temperatrue_data_float_buf[0] =qrand()%10;
    double t = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot->xAxis->setLabel("x1");
    customPlot->yAxis->setLabel("y1");
    customPlot->yAxis->setRange(trend_min, trend_max);
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot->xAxis->setRange( t,Count, Qt::AlignRight);
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot->graph(0)->setAntialiasedFill(false);
    customPlot->graph(0)->addData(t+0.25, temperatrue_data_float_buf[0]);
    customPlot->replot();
}
void MainWindow::setupRealtimecurve2(QCustomPlot *customPlot2)
{

    double Count2=1500;
    //double temperatrue_data_float_buf[1] =qrand()%10;
    double t2 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot2->xAxis->setLabel("x2");
    customPlot2->yAxis->setLabel("y2");
    customPlot2->yAxis->setRange(trend_min, trend_max);
    customPlot2->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot2->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot2->xAxis->setRange( t2,Count2, Qt::AlignRight);
    customPlot2->addGraph(); // blue line
    customPlot2->graph(0)->setPen(QPen(Qt::blue));
    customPlot2->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot2->graph(0)->setAntialiasedFill(false);
    customPlot2->graph(0)->addData(t2+0.25, temperatrue_data_float_buf[1]);
    customPlot2->replot();
}
void MainWindow::setupRealtimecurve3(QCustomPlot *customPlot3)
{

    double Count3=1500;
    //double temperatrue_data_float_buf[2] =qrand()%10;
    double t3 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot3->xAxis->setLabel("x3");
    customPlot3->yAxis->setLabel("y3");
    customPlot3->yAxis->setRange(trend_min, trend_max);
    customPlot3->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot3->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot3->xAxis->setRange( t3,Count3, Qt::AlignRight);
    customPlot3->addGraph(); // blue line
    customPlot3->graph(0)->setPen(QPen(Qt::blue));
    customPlot3->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot3->graph(0)->setAntialiasedFill(false);
    customPlot3->graph(0)->addData(t3+0.25, temperatrue_data_float_buf[2]);
    customPlot3->replot();
}
void MainWindow::setupRealtimecurve4(QCustomPlot *customPlot4)
{

    double Count4=1500;
    //double temperatrue_data_float_buf[3] =qrand()%10;
    double t4 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot4->xAxis->setLabel("x4");
    customPlot4->yAxis->setLabel("y4");
    customPlot4->yAxis->setRange(trend_min, trend_max);
    customPlot4->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot4->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot4->xAxis->setRange( t4,Count4, Qt::AlignRight);
    customPlot4->addGraph(); // blue line
    customPlot4->graph(0)->setPen(QPen(Qt::blue));
    customPlot4->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot4->graph(0)->setAntialiasedFill(false);
    customPlot4->graph(0)->addData(t4+0.25, temperatrue_data_float_buf[3]);
    customPlot4->replot();
}
void MainWindow::setupRealtimecurve5(QCustomPlot *customPlot5)
{

    double Count5=1500;
    //double temperatrue_data_float_buf[4] =qrand()%10;
    double t5 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot5->xAxis->setLabel("x5");
    customPlot5->yAxis->setLabel("y5");
    customPlot5->yAxis->setRange(trend_min, trend_max);
    customPlot5->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot5->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot5->xAxis->setRange( t5,Count5, Qt::AlignRight);
    customPlot5->addGraph(); // blue line
    customPlot5->graph(0)->setPen(QPen(Qt::blue));
    customPlot5->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot5->graph(0)->setAntialiasedFill(false);
    customPlot5->graph(0)->addData(t5+0.25, temperatrue_data_float_buf[4]);
    customPlot5->replot();
}
void MainWindow::setupRealtimecurve6(QCustomPlot *customPlot6)
{

    double Count6=1500;
    //double temperatrue_data_float_buf[5] =qrand()%10;
    double t6 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot6->xAxis->setLabel("x6");
    customPlot6->yAxis->setLabel("y6");
    customPlot6->yAxis->setRange(trend_min, trend_max);
    customPlot6->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot6->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot6->xAxis->setRange( t6,Count6, Qt::AlignRight);
    customPlot6->addGraph(); // blue line
    customPlot6->graph(0)->setPen(QPen(Qt::blue));
    customPlot6->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot6->graph(0)->setAntialiasedFill(false);
    customPlot6->graph(0)->addData(t6+0.25, temperatrue_data_float_buf[5]);
    customPlot6->replot();
}
void MainWindow::setupRealtimecurve7(QCustomPlot *customPlot7)
{

    double Count7=1500;
    //double temperatrue_data_float_buf[6] =qrand()%10;
    double t7 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot7->xAxis->setLabel("x7");
    customPlot7->yAxis->setLabel("y7");
    customPlot7->yAxis->setRange(trend_min, trend_max);
    customPlot7->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot7->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot7->xAxis->setRange( t7,Count7, Qt::AlignRight);
    customPlot7->addGraph(); // blue line
    customPlot7->graph(0)->setPen(QPen(Qt::blue));
    customPlot7->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot7->graph(0)->setAntialiasedFill(false);
    customPlot7->graph(0)->addData(t7+0.25, temperatrue_data_float_buf[6]);
    customPlot7->replot();
}
void MainWindow::setupRealtimecurve8(QCustomPlot *customPlot8)
{

    double Count8=1500;
    //double temperatrue_data_float_buf[7] =qrand()%10;
    double t8 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    customPlot8->xAxis->setLabel("x8");
    customPlot8->yAxis->setLabel("y8");
    customPlot8->yAxis->setRange(trend_min, trend_max);
    customPlot8->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot8->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot8->xAxis->setRange( t8,Count8, Qt::AlignRight);
    customPlot8->addGraph(); // blue line
    customPlot8->graph(0)->setPen(QPen(Qt::blue));
    customPlot8->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot8->graph(0)->setAntialiasedFill(false);
    customPlot8->graph(0)->addData(t8+0.25, temperatrue_data_float_buf[7]);
    customPlot8->replot();
}

//柱状图的初始化
void MainWindow::init_strength_ui()
{
    QString strength_bye[8]={"通道1","通道2","通道3","通道4","通道5","通道6","通道7","通道8"};
    for(int i=0;i<8;i++)strength_labels[i]=strength_bye[i];
    this->ui->widget_singal_strength->yAxis->setLabel("信号强度/dBM");
   // this->ui->widget_singal_strength->xAxis->setLabel("通道");

    //添加完了绘制的柱状图，接下来添加标签，要想完全自己定义标签，需要先执行以下代码关闭默认的底部标签自动生成
    this->ui->widget_singal_strength->xAxis->setAutoTicks(false);//以下这三句为关闭默认坐标取值
    this->ui->widget_singal_strength->xAxis->setAutoTickLabels(false);
    this->ui->widget_singal_strength->xAxis->setAutoTickStep(false);


    //设置坐标轴显示范围,否则我们只能看到默认的范围
    this->ui->widget_singal_strength->xAxis->setRange(0,8);
    this->ui->widget_singal_strength->yAxis->setRange(0,120);

    //this->ui->widget->rescaleAxes();//它会自动设置最合适的显示范围，非常简便。
    //this->ui->widget->setAlignment(Qt::AlignHCenter);
    this->ui->widget_singal_strength->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //设置坐标背景色
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(200, 200, 200));
    //axisRectGradient.setColorAt(1, QColor(130, 130, 130));
    this->ui->widget_singal_strength->axisRect()->setBackground(axisRectGradient);

    //设置画布背景色
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(200, 200, 200));
    //plotGradient.setColorAt(1, QColor(200, 200, 200));
    this->ui->widget_singal_strength->setBackground(plotGradient);
    this->ui->widget_singal_strength->replot();
    read_signal_strenth();
}



void MainWindow::timer_sampleEvent( QTimerEvent * )//定时器，
{

}
void MainWindow::timer_sampleUpDate()
{
    send_serial_data();
}


unsigned short int s_count=0;
void MainWindow::timer_serialportEvent( QTimerEvent * )//定时器，
{

}

void MainWindow::timer_serialportUpDate()
{
    unsigned short int temp16=0;
    QPixmap a;
    if(s_count<5)s_count=2;
    temp16=BUILD_UINT16(serial_data_buf[s_count-2], serial_data_buf[s_count-1]);
    if(CRC_16(serial_data_buf, s_count-2)==temp16)
    {
        if( status_buf[0]==0){a.load(":/res/image/light3.png");ui->com_status_light->setPixmap(a);}
        else{a.load(":/res/image/light4.png"); ui->com_status_light->setPixmap(a);}
        if(status_buf[0]==0)status_buf[0]=1;
        else status_buf[0]=0;
        if(serial_data_buf[1]==Modbus_data_read_fuction)read_temperature();
        else if(serial_data_buf[1]==Address_read_function){device_ID=serial_data_buf[0];ui->lineEdit_readid->setText(tr("ID:%1").arg((int)device_ID));}
        else if(serial_data_buf[1]==battery_read_function)read_battery_dl();
        else if(serial_data_buf[1]==strength_read_fuction)read_signal_strenth();//strength_Pre[0]=34;
    }
   else {a.load(":/res/image/light0.png"); ui->com_status_light->setPixmap(a);}
   //ui->com_status_light->setStyleSheet("QPushButton{border-image: url(:/res/image/2.png.jpg);}");
   s_count=0;
   memset(serial_data_buf,0,sizeof(serial_data_buf));
   timer_serialport->stop();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //QPainter painter(this);
    //painter.drawPixmap(m_pixmapBg.rect(), m_pixmapBg);
    QBitmap bitmap(this->size());
    bitmap.fill();
    QPainter painter(&bitmap);
    painter.setBrush(QBrush(Qt::black));
    painter.drawRoundedRect(bitmap.rect(),5,5);
    setMask(bitmap);

    QPixmap pixmap(this->size());
    pixmap.fill();
    painter.end();
    painter.begin(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing,true);
    QBrush brush;
    brush.setStyle(Qt::TexturePattern);
    brush.setTextureImage(QImage(bkPicName));
    painter.setBrush(brush);
    painter.setPen(Qt::black);
    painter.drawRoundedRect(rect(),5,5);
    painter.end();
    painter.begin(this);
    painter.drawPixmap(this->rect(),pixmap);

}
/*
//鼠标只对图片上方有用
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //m_pointPress = e->globalPos();
    //m_pointStart = m_pointPress - this->pos();
    if (e->button() == Qt::LeftButton)
    {
        m_pointPress = e->globalPos() - frameGeometry().topLeft();
        e->accept();
    }

}

//鼠标只对图片上方有用
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    //this->move(e->globalPos() - m_pointStart);
    if (e->buttons() & Qt::LeftButton)
    //if (event->button() == Qt::LeftButton) // not work
    {
        if (m_pointPress.y() < 110)
        {

                move(e->globalPos() - m_pointPress);

        }
        e->accept();
    }
}
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
*/
//最上面功能初始化
void MainWindow::anniuyangshi()
{
    //设置按钮样式
    ui->closeBtn->setStyleSheet("QPushButton{border-image: url(://res/title_button/close2.png);}");
                           //  "QPushButton:hover{border-image: url(://res/image/close2.png);}"
                           //  "QPushButton:pressed{border-image: url(://res/image/close2.png);}"
    ui->nomalBtn->setStyleSheet("QPushButton{border-image: url(://res/title_button/max2.png);}");
                           //"QPushButton:hover{border-image: url(://res/image/max2.png);}"
                           //"QPushButton:pressed{border-image: url(://res/image/max2.png);}"
    ui->minBtn->setStyleSheet("QPushButton{border-image: url(://res/title_button/min2.png);}");
                           //"QPushButton:hover{border-image: url(://res/image/min2.png);}"
                          // "QPushButton:pressed{border-image: url(://res/image/min2.png);}"
    ui->menuBtn->setStyleSheet("QPushButton{border-image: url(://res/title_button/cai2.png);}");
                           //"QPushButton:hover{border-image: url(://res/image/cai2.png);}"
                          // "QPushButton:pressed{border-image: url(://res/image/cai2.png);}"

    //右上角安工大标号
    ui->pushButton_9->setStyleSheet("QPushButton{border-image: url(:/res/logo/log.jpg);}");
    //设置功能按钮样式
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(:/res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(:/res/logo/zhuhuamian1.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(:/res/logo/zhifangtu1.png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(:/res/logo/table1.png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(:/res/logo/guzhangzhenduan1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(:/res/logo/baojing1.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(:/res/logo/chuankou1.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(:/res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(:/res/logo/guanyu1.png);}");
}
//?????????????????
void MainWindow::on_pushButton_clicked()
{
     setCurrentWidget();
     debug_mode_flag=0;
     ui->title->setText("通讯接口设置");
     ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi11.png);}");
     ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
     ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
     ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
     ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
     ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
     ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou1.png);}");
     ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
     ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");
}

void MainWindow::on_pushButton_2_clicked()
{
    setCurrentWidget();
    debug_mode_flag=0;
    ui->title->setText("实时曲线显示");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian11.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou1.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");
}

void MainWindow::on_pushButton_3_clicked()
{
    setCurrentWidget();
    debug_mode_flag=0;
    ui->title->setText("直方图统计");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu11.png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou1.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");
}

void MainWindow::on_pushButton_4_clicked()
{
    setCurrentWidget();
    ui->title->setText("网络通讯状态");
    debug_mode_flag=0;
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan11.png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou1.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");
}

void MainWindow::on_pushButton_5_clicked()
{
    setCurrentWidget();
    debug_mode_flag=0;
    ui->title->setText("水温差及热负荷");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table11.png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou1.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");

}

void MainWindow::on_pushButton_6_clicked()
{
    setCurrentWidget();
    debug_mode_flag=0;
    ui->title->setText("温度报警设置");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing11.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou1.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");
}

void MainWindow::on_pushButton_7_clicked()
{
    setCurrentWidget();
    //debug_mode_flag=1;
    //timer_sample->stop(); //停止采集
    //ui->start_sample->setEnabled(true); //
    ui->title->setText("串口调试助手");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou11.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");
}

void MainWindow::on_pushButton_8_clicked()
{
     setCurrentWidget();
     debug_mode_flag=0;
     ui->title->setText("采集终端监控");
     ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
     ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
     ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
     ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
     ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
     ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
     ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou1.png);}");
     ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance11.png);}");//
     ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu1.png);}");
}


void MainWindow::on_pushButton_19_clicked()
{
    setCurrentWidget();
    debug_mode_flag=0;
    ui->title->setText("系统总体介绍");
    ui->pushButton->setStyleSheet("QPushButton{border-image: url(://res/logo/shezhi1.png);}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image: url(://res/logo/zhuhuamian1.png);}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image: url(://res/logo/zhifangtu1 .png);}");
    ui->pushButton_4->setStyleSheet("QPushButton{border-image: url(://res/logo/guzhangzhenduan1.png);}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image: url(://res/logo/table1.png);}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image: url(://res/logo/baojing1.png);}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image: url(://res/logo/chuankou11.png);}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image: url(://res/logo/zhongduanjiance1.png);}");
    ui->pushButton_19->setStyleSheet("QPushButton{border-image: url(://res/logo/guanyu11.png);}");//
}
void MainWindow::setNomalStyle()
{
    /*QFile styleSheet(":/res/qss/style_360.qss");
    if (!styleSheet.open(QIODevice::ReadOnly))
    {
        qWarning("Can't open the style sheet file.");
        return;
    }
    qApp->setStyleSheet(styleSheet.readAll());*/
}



void MainWindow::showSkinWidget()
{
    skinWidget *skinW=new skinWidget(bkPicName);
    skinW->setAttribute(Qt::WA_DeleteOnClose);
    connect(skinW,SIGNAL(changeSkin(QString)),this,SLOT(setPicName(QString)));
    QPoint p=rect().topRight();
    p.setX(p.x()-150-350);
    p.setY(p.y()+30);
    skinW->move(this->mapToGlobal(p));
    skinW->show();
}

void MainWindow::setPicName(QString picName)
{
    bkPicName=picName;
    update();
}

void MainWindow::on_menuBtn_clicked()
{
   showSkinWidget();
   ///背景加载;
   //bkPicName=":/res/skin/classic.jpg";
   //m_pixmapBg.load(bkPicName);
   //qreal width = m_pixmapBg.width(); //获得以前图片的宽和高
   //qreal height = m_pixmapBg.height();
   //int a=MainWindow::height();
   //int b=MainWindow::width();
   //m_pixmapBg = m_pixmapBg.scaled(b,a);
   //m_menu->exec(this->mapToGlobal(QPoint(1000, 20)));
}

void MainWindow::initData()
{

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    bkPicName=":/res/skin/classic.jpg";   //初始化背景图片

#ifdef Q_OS_LINUX
   ui->portNameComboBox->addItem( "ttyUSB0");
   ui->portNameComboBox->addItem( "ttyUSB1");
   ui->portNameComboBox->addItem( "ttyUSB2");
   ui->portNameComboBox->addItem( "ttyUSB3");
   ui->portNameComboBox->addItem( "ttyS0");
   ui->portNameComboBox->addItem( "ttyS1");
   ui->portNameComboBox->addItem( "ttyS2");
   ui->portNameComboBox->addItem( "ttyS3");
   ui->portNameComboBox->addItem( "ttyS4");
   ui->portNameComboBox->addItem( "ttyS5");
   ui->portNameComboBox->addItem( "ttyS6");
#elif defined (Q_OS_WIN)
   ui->portNameComboBox->addItem("COM1");
   ui->portNameComboBox->addItem("COM2");
   ui->portNameComboBox->addItem("COM3");
   ui->portNameComboBox->addItem("COM4");
   ui->portNameComboBox->addItem("COM5");
   ui->portNameComboBox->addItem("COM6");
   ui->portNameComboBox->addItem("COM7");
   ui->portNameComboBox->addItem("COM8");
   ui->portNameComboBox->addItem("COM9");
   ui->portNameComboBox->addItem("COM10");
   ui->portNameComboBox->addItem("COM11");
   ui->portNameComboBox->addItem("COM12");
   ui->portNameComboBox->addItem("COM13");
   ui->portNameComboBox->addItem("COM14");
   ui->portNameComboBox->addItem("COM15");
#endif
    // m_AactionAboutQt = new QAction(tr("&About Qt"), this);
    // connect(m_AactionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    // m_menu->addAction(m_AactionAboutQt);
    ///背景加载;
    //m_pixmapBg.load(":/res/image/skin/22_big.jpg");
    //qreal width = m_pixmapBg.width(); //获得以前图片的宽和高
    //qreal height = m_pixmapBg.height();
    //int a=MainWindow::height();
    //int b=MainWindow::width();
    //m_pixmapBg = m_pixmapBg.scaled(b,a);
    ui->stackedWidget->setAutoFillBackground(1);
    QPalette palette;
    palette.setBrush(QPalette::Window,QBrush(QColor(255,255,255,20)));
    setPalette(palette);
    ui->stackedWidget->setPalette(palette);
    m_vecBtn.push_back(ui->pushButton);
    m_vecBtn.push_back(ui->pushButton_2);
    m_vecBtn.push_back(ui->pushButton_3);
    m_vecBtn.push_back(ui->pushButton_4);
    m_vecBtn.push_back(ui->pushButton_5);
    m_vecBtn.push_back(ui->pushButton_6);
    m_vecBtn.push_back(ui->pushButton_7);
    m_vecBtn.push_back(ui->pushButton_8);
    m_vecBtn.push_back(ui->pushButton_19);

    for (int i = 0; i != m_vecBtn.size(); ++i)
    {
        ///功能选择键只有一个被选中;
        m_vecBtn[i]->setCheckable(true);
        m_vecBtn[i]->setAutoExclusive(true);
    }
     setNomalStyle();
}


void MainWindow::on_nomalBtn_clicked()
{
     int a;
     int b;
    static bool isMax=false;
    if(isMax)
    {
        //move(normalTopLeft);
        resize(1200,720);
        isMax=false;
        a=MainWindow::height();
        b=MainWindow::width();
        m_pixmapBg = m_pixmapBg.scaled(b,a);
    }
    else
    {
        //normalTopLeft=this->pos();
        setGeometry(QApplication::desktop()->availableGeometry());
        isMax=true;
        a=MainWindow::height();
        b=MainWindow::width();
        m_pixmapBg = m_pixmapBg.scaled(b,a);
    }
}
void MainWindow::setCurrentWidget()
{
    for (int i = 0; i != m_vecBtn.size(); ++i)
    {
        if ( m_vecBtn[i]->isChecked() )
        {
          ui->stackedWidget->setCurrentIndex(i);
        }
    }
}


void MainWindow::startInit(){
    setActionsEnabled(false);
    ui->delayspinBox->setEnabled(false);
    ui->sendmsgBtn->setEnabled(false);
    ui->sendMsgLineEdit->setEnabled(false);
    //ui->obocheckBox->setEnabled(false);
    //ui->actionAdd->setEnabled(true);

    //初始化连续发送计时器计时间隔
    obotimerdly = OBO_TIMER_INTERVAL;

    //设置连续发送计时器
    obotimer = new QTimer(this);
    connect(obotimer, SIGNAL(timeout()), this, SLOT(sendMsg()));

}

void MainWindow::setActionsEnabled(bool status)
{
    //ui->actionSave->setEnabled(status);
    //ui->actionClose->setEnabled(status);
    //ui->actionLoadfile->setEnabled(status);
    //ui->actionCleanPort->setEnabled(status);
    //ui->actionWriteToFile->setEnabled(status);
}

void MainWindow::setComboBoxEnabled(bool status)
{
    ui->portNameComboBox->setEnabled(status);
    ui->baudRateComboBox->setEnabled(status);
    ui->dataBitsComboBox->setEnabled(status);
    ui->parityComboBox->setEnabled(status);
    ui->stopBitsComboBox->setEnabled(status);
}

//打开串口

void MainWindow::on_actionOpen_triggered()
{
    QString portName = ui->portNameComboBox->currentText();   //获取串口名
#ifdef Q_OS_LINUX
    myCom = new QextSerialPort("/dev/" + portName);
#elif defined (Q_OS_WIN)
    myCom = new QextSerialPort(portName);
#endif
    connect(myCom, SIGNAL(readyRead()), this, SLOT(readMyCom()));    //

    //设置波特率
    myCom->setBaudRate((BaudRateType)ui->baudRateComboBox->currentText().toInt());

    //设置数据位
    myCom->setDataBits((DataBitsType)ui->dataBitsComboBox->currentText().toInt());

    //设置校验
    switch(ui->parityComboBox->currentIndex()){
    case 0:
         myCom->setParity(PAR_ODD);
         break;
    case 1:
        myCom->setParity(PAR_EVEN);
        break;
    case 2:
        myCom->setParity(PAR_NONE);
        break;
    default:
        myCom->setParity(PAR_NONE);
        qDebug("set to default : PAR_NONE");
        break;
    }

    //设置停止位
    switch(ui->stopBitsComboBox->currentIndex()){
    case 0:
        myCom->setStopBits(STOP_1);
        break;
    case 1:
 #ifdef Q_OS_WIN
        myCom->setStopBits(STOP_1_5);
 #endif
        break;
    case 2:
        myCom->setStopBits(STOP_2);
        break;
    default:
        myCom->setStopBits(STOP_1);
        qDebug("set to default : STOP_1");
        break;
    }

    //设置数据流控制
    myCom->setFlowControl(FLOW_OFF);
    //设置延时
    myCom->setTimeout(TIME_OUT);

    if(myCom->open(QIODevice::ReadWrite)){
        QMessageBox::information(this, tr("打开成功"), tr("已成功打开串口") + portName, QMessageBox::Ok);
        //界面控制
        ui->sendmsgBtn->setEnabled(true);
        setComboBoxEnabled(false);

        ui->sendMsgLineEdit->setEnabled(true);

        //ui->actionOpen->setEnabled(false);
        ui->sendMsgLineEdit->setFocus();
        //ui->obocheckBox->setEnabled(true);
        //ui->actionAdd->setEnabled(false);

        setActionsEnabled(true);
    }else{
        QMessageBox::critical(this, tr("打开失败"), tr("未能打开串口 ") + portName + tr("\n该串口设备不存在或已被占用"), QMessageBox::Ok);
        return;
    }

    //ui->statusBar->showMessage(tr("打开串口成功"));
}

unsigned short int MainWindow::CRC_16(unsigned char * pchMsg, unsigned short int wDataLen)
{
  unsigned short int wCRC = 0xFFFF;
  unsigned short int i;
  unsigned char   chChar;
  for (i=0;i<wDataLen;i++)
   {
      chChar= *pchMsg++;
      wCRC  = wCRCTalbeAbs[(chChar^wCRC)&15]^(wCRC>>4);
      wCRC  = wCRCTalbeAbs[((chChar>>4)^wCRC) & 15]^(wCRC>>4);
   }
  return wCRC;
}
//读取数据


void MainWindow::readMyCom()
{
    QByteArray temp = myCom->readAll();
    QString buf;
    //unsigned short int temp16=0;
    //memset(serial_data_buf,0,sizeof(serial_data_buf));
    if(!temp.isEmpty()){
            ui->textBrowser->setTextColor(Qt::black);
            if(ui->ccradioButton->isChecked()){
                buf = temp;
            }else if(ui->chradioButton->isChecked()){
                //QString str;
                for(int i = 0; i < temp.count(); i++){
                    QString s;
                    s.sprintf("%02x", (unsigned char)temp.at(i));
                    buf += s;
                    serial_data_buf[s_count++]=(unsigned char)temp.at(i);
                }  
            }

        if(debug_mode_flag==1)
        {
           ui->textBrowser->setText(ui->textBrowser->document()->toPlainText() + buf);
           QTextCursor cursor = ui->textBrowser->textCursor();
           cursor.movePosition(QTextCursor::End);
           ui->textBrowser->setTextCursor(cursor);
           ui->recvbyteslcdNumber->display(ui->recvbyteslcdNumber->value() + temp.size());
           // ui->statusBar->showMessage(tr("成功读取%1字节数据").arg(temp.size()));
        }
    }
}

//发送数据
void MainWindow::sendMsg()
{
    QByteArray buf;
    if(ui->sendAsHexcheckBox->isChecked()){
        QString str;
        bool ok;
        char data;
        QStringList list;
        str = ui->sendMsgLineEdit->text();
        list = str.split(" ");
        for(int i = 0; i < list.count(); i++){
            if(list.at(i) == " ")
                continue;
            if(list.at(i).isEmpty())
                continue;
            data = (char)list.at(i).toInt(&ok, 16);
            if(!ok){
                QMessageBox::information(this, tr("提示消息"), tr("输入的数据格式有错误！"), QMessageBox::Ok);
                if(obotimer != NULL)
                    obotimer->stop();
                //ui->sendmsgBtn->setText(tr("发送"));
                //ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/send.png"));
                return;
            }
            buf.append(data);
        }
    }else{
#if QT_VERSION < 0x050000
        buf = ui->sendMsgLineEdit->text().toAscii();
#else
        buf = ui->sendMsgLineEdit->text().toLocal8Bit();
#endif
    }
    //发送数据
    myCom->write(buf);
    //ui->statusBar->showMessage(tr("发送数据成功"));
    //界面控制
    //ui->textBrowser->setTextColor(Qt::lightGray);
}
void MainWindow::send_serial_data()
{
    QByteArray buf;
    for(int i = 0; i <8; i++)buf.append(protocol_data_buf[i]);
    myCom->write(buf);
    timer_serialport->start(200);//设定接收时间 200ms
    //sendMsg();
}
//发送数据按钮
void MainWindow::on_sendmsgBtn_clicked()
{
    timer_sample->stop();
    ui->start_sample->setEnabled(true);
    debug_mode_flag=1;
    //如果当前正在连续发送数据，暂停发送
    if(ui->sendmsgBtn->text() == tr("暂停")){
        obotimer->stop();
        ui->sendmsgBtn->setText(tr("发送"));
        ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/send.png"));
        return;
    }
    //如果发送数据为空，给出提示并返回
    if(ui->sendMsgLineEdit->text().isEmpty()){
        QMessageBox::information(this, tr("提示消息"), tr("没有需要发送的数据"), QMessageBox::Ok);
        return;
    }

    //如果不是连续发送
    //if(!ui->obocheckBox->isChecked()){
        ui->sendMsgLineEdit->setFocus();
        //发送数据
        sendMsg();
    //}
    /*
    else{ //连续发送
        obotimer->start(obotimerdly);
        ui->sendmsgBtn->setText(tr("暂停"));
        ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/pause.png"));

    }*/
}
//清空记录
void MainWindow::on_clearUpBtn_clicked()
{
    ui->textBrowser->clear();
   // ui->statusBar->showMessage(tr("记录已经清空"));
}
void MainWindow::on_open_Sciport_clicked()
{
   on_actionOpen_triggered();
   ui->open_Sciport->setEnabled(false);
   ui->close_Sciport->setEnabled(true);
   //ui->textBrowser->clear();
   // ui->statusBar->showMessage(tr("记录已经清空"));
}
void MainWindow::on_close_Sciport_clicked()
{
    myCom->close();
    delete myCom;
    myCom = NULL;

    ui->sendmsgBtn->setEnabled(false);

    setComboBoxEnabled(true);

   // ui->actionOpen->setEnabled(true);
    ui->sendMsgLineEdit->setEnabled(false);
    //ui->obocheckBox->setEnabled(false);
    //ui->actionAdd->setEnabled(true);

    setActionsEnabled(false);

   // ui->actionWriteToFile->setChecked(false);
   // ui->statusBar->showMessage(tr("??????????"));
    ui->open_Sciport->setEnabled(true);
    ui->close_Sciport->setEnabled(false);
}

void MainWindow::read_temperature()
{

    unsigned short int i=0,temp16=0;
    float a=0;
    for(i=0;i<8;i++)
    {
       temp16= serial_data_buf[2*i+3];
       temp16<<=8;
       temp16+= serial_data_buf[2*i+4];
       a=(float)temp16/100;
       temperatrue_data_float_buf[i]=a;
    }

}

void MainWindow::read_battery_dl()
{
    float a=0;
    for(char i=0;i<8;i++)
    {
       a=(float)serial_data_buf[i+3];
       battery_data_buf[i]=a/50;
    }
    QString str1;
    float min_dianya=3.0;
    float max_dianya=4.3;
    if(battery_data_buf[0]>min_dianya)ui->progressBar_bq->setValue((int)((battery_data_buf[0]-min_dianya)*100/(max_dianya-min_dianya)));
    else ui->progressBar_bq->setValue(0);
    ui->label_dyvalue->setText(str1.setNum(battery_data_buf[0])+"V");

    if(battery_data_buf[1]>min_dianya)ui->progressBar_bq_2->setValue((int)((battery_data_buf[1]-min_dianya)*100/(max_dianya-min_dianya)));
    else ui->progressBar_bq_2->setValue(0);
    ui->label_dyvalue_2->setText(str1.setNum(battery_data_buf[1])+"V");

    if(battery_data_buf[2]>min_dianya)ui->progressBar_bq_3->setValue((int)((battery_data_buf[2]-min_dianya)*100/(max_dianya-min_dianya)));
        else ui->progressBar_bq_3->setValue(0);
    ui->label_dyvalue_3->setText(str1.setNum(battery_data_buf[2])+"V");

    if(battery_data_buf[3]>min_dianya)ui->progressBar_bq_4->setValue((int)((battery_data_buf[3]-min_dianya)*100/(max_dianya-min_dianya)));
    else ui->progressBar_bq_4->setValue(0);
    ui->label_dyvalue_4->setText(str1.setNum(battery_data_buf[3])+"V");

    if(battery_data_buf[4]>min_dianya)ui->progressBar_bq_5->setValue((int)((battery_data_buf[4]-min_dianya)*100/(max_dianya-min_dianya)));
    else ui->progressBar_bq_5->setValue(0);
    ui->label_dyvalue_5->setText(str1.setNum(battery_data_buf[4])+"V");

    if(battery_data_buf[5]>min_dianya)ui->progressBar_bq_6->setValue((int)((battery_data_buf[5]-min_dianya)*100/(max_dianya-min_dianya)));
    else ui->progressBar_bq_6->setValue(0);
    ui->label_dyvalue_6->setText(str1.setNum(battery_data_buf[5])+"V");

    if(battery_data_buf[6]>min_dianya)ui->progressBar_bq_7->setValue((int)((battery_data_buf[6]-min_dianya)*100/(max_dianya-min_dianya)));
    else ui->progressBar_bq_7->setValue(0);
    ui->label_dyvalue_7->setText(str1.setNum(battery_data_buf[6])+"V");

    if(battery_data_buf[7]>min_dianya)ui->progressBar_bq_8->setValue((int)((battery_data_buf[7]-min_dianya)*100/(max_dianya-min_dianya)));
    else ui->progressBar_bq_8->setValue(0);
    ui->label_dyvalue_8->setText(str1.setNum(battery_data_buf[7])+"V");
}
void MainWindow::read_signal_strenth()
{
char a=0;
   for(char i=0;i<8;i++)
   {
      if(serial_data_buf[i+3]>0)a=120-serial_data_buf[i+3];
      singal_strength_data_buf[i]=(unsigned char)a;
   }
   this->ui->widget_singal_strength->clearPlottables();//清除之前保留在坐标轴的数据图形
   //柱状图我们需要两个数据，一个是每个柱的相应标签(QString)，一个就是其值(double)
   for(char i=0;i<8;i++)strength_Pre[i]=singal_strength_data_buf[i];
   QCPBars* strength_bars=new QCPBars(this->ui->widget_singal_strength->xAxis,this->ui->widget_singal_strength->yAxis);
   QVector<double> strength_index(8);
   for(int i=0;i<8;++i)
   strength_index[i]=i+0.39;
   strength_bars->setData(strength_index,strength_Pre);
   this->ui->widget_singal_strength->addPlottable(strength_bars);

       this->ui->widget->addPlottable(strength_bars);
       double widget_wid=this->ui->widget_singal_strength->xAxis->range().upper-this->ui->widget_singal_strength->xAxis->range().lower;
       double widget_cl=strength_bars->width()+(1.0*widget_wid-strength_bars->width()*5)/17;//这句定义x坐标下标示的个数

       QVector<double> strength_coor;
       for(int i=0;i<8;++i)
       {
           strength_coor.append(this->ui->widget_singal_strength->xAxis->range().lower+i*widget_cl+strength_bars->width()/2);

       }
       this->ui->widget_singal_strength->xAxis->setTickVector(strength_coor);
       this->ui->widget_singal_strength->xAxis->setTickVectorLabels(strength_labels);
       strength_bars->setBrush(QColor(0,170,0,255));
       this->ui->widget_singal_strength->replot();
       }
void MainWindow::on_tclear_clicked()
{
  debug_mode_flag=1;
}


//计数器清零
void MainWindow::on_actionClearBytes_triggered()
{
    if(ui->recvbyteslcdNumber->value() == 0){
        QMessageBox::information(this, tr("提示消息"), tr("貌似已经清零了呀:)"), QMessageBox::Ok);
    }else{
        ui->recvbyteslcdNumber->display(0);
       // ui->statusBar->showMessage(tr("计数器已经清零"));
    }
}


//清空串口中的I/O数据
void MainWindow::on_actionCleanPort_triggered()
{
    myCom->flush();
}

void MainWindow::on_start_sample_clicked()
{
    debug_mode_flag=0;
    timer_sample->start(ui->delayspinBox->value());
    ui->start_sample->setEnabled(false);
    ui->stop_sample->setEnabled(true);
    unsigned int temp16=0;
    protocol_data_buf[0]=device_ID;
    protocol_data_buf[1]=Modbus_data_read_fuction;
    protocol_data_buf[2]=0x00;
    protocol_data_buf[3]=0x00;
    protocol_data_buf[4]=0x00;
    protocol_data_buf[5]=0x08;
    temp16=CRC_16(protocol_data_buf,6);
    protocol_data_buf[6]= LO_UINT16(temp16);
    protocol_data_buf[7]= HI_UINT16(temp16);


}
void MainWindow::on_stop_sample_clicked()
{
   timer_sample->stop();
   ui->start_sample->setEnabled(true);
   ui->stop_sample->setEnabled(false);
}
void MainWindow::on_delayspinBox_editingFinished()
{   
    device_sample_time=ui->delayspinBox->value();
    timer_sample->start(device_sample_time);
}
void MainWindow::on_verticalSlider_bj_T_valueChanged(int value)
{
    QString a;
    baojing_T_value[0]=ui->verticalSlider_bj_T->value();
    ui->label_baoj_setT->setText(a.setNum(baojing_T_value[0])+" ℃");
}
void MainWindow::on_verticalSlider_bj_T_2_valueChanged(int value)
{
    QString a;
    baojing_T_value[1]=ui->verticalSlider_bj_T_2->value();
    ui->label_baoj_setT_2->setText(a.setNum(baojing_T_value[1])+" ℃");
}
void MainWindow::on_verticalSlider_bj_T_3_valueChanged(int value)
{
    QString a;
    baojing_T_value[2]=ui->verticalSlider_bj_T_3->value();
    ui->label_baoj_setT_3->setText(a.setNum(baojing_T_value[2])+" ℃");
}
void MainWindow::on_verticalSlider_bj_T_4_valueChanged(int value)
{
    QString a;
    baojing_T_value[3]=ui->verticalSlider_bj_T_4->value();
    ui->label_baoj_setT_4->setText(a.setNum(baojing_T_value[3])+" ℃");
}
void MainWindow::on_verticalSlider_bj_T_5_valueChanged(int value)
{
    QString a;
    baojing_T_value[4]=ui->verticalSlider_bj_T_5->value();
    ui->label_baoj_setT_5->setText(a.setNum(baojing_T_value[4])+" ℃");
}
void MainWindow::on_verticalSlider_bj_T_6_valueChanged(int value)
{
    QString a;
    baojing_T_value[5]=ui->verticalSlider_bj_T_6->value();
    ui->label_baoj_setT_6->setText(a.setNum(baojing_T_value[5])+" ℃");
}
void MainWindow::on_verticalSlider_bj_T_7_valueChanged(int value)
{
    QString a;
    baojing_T_value[6]=ui->verticalSlider_bj_T_7->value();
    ui->label_baoj_setT_7->setText(a.setNum(baojing_T_value[6])+" ℃");
}
void MainWindow::on_verticalSlider_bj_T_8_valueChanged(int value)
{
    QString a;
    baojing_T_value[7]=ui->verticalSlider_bj_T_8->value();
    ui->label_baoj_setT_8->setText(a.setNum(baojing_T_value[7])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_valueChanged(int value)
{
    QString a;
    baojing_TC_value[0]=ui->verticalSlider_bj_TC->value();
    ui->label_baoj_setTC->setText(a.setNum(baojing_TC_value[0])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_2_valueChanged(int value)
{
    QString a;
    baojing_TC_value[1]=ui->verticalSlider_bj_TC_2->value();
    ui->label_baoj_setTC_2->setText(a.setNum(baojing_TC_value[1])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_3_valueChanged(int value)
{
    QString a;
    baojing_TC_value[2]=ui->verticalSlider_bj_TC_3->value();
    ui->label_baoj_setTC_3->setText(a.setNum(baojing_TC_value[2])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_4_valueChanged(int value)
{
    QString a;
    baojing_TC_value[3]=ui->verticalSlider_bj_TC_4->value();
    ui->label_baoj_setTC_4->setText(a.setNum(baojing_TC_value[3])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_5_valueChanged(int value)
{
    QString a;
    baojing_TC_value[4]=ui->verticalSlider_bj_TC_5->value();
    ui->label_baoj_setTC_5->setText(a.setNum(baojing_TC_value[4])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_6_valueChanged(int value)
{
    QString a;
    baojing_TC_value[5]=ui->verticalSlider_bj_TC_6->value();
    ui->label_baoj_setTC_6->setText(a.setNum(baojing_TC_value[5])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_7_valueChanged(int value)
{
    QString a;
    baojing_TC_value[6]=ui->verticalSlider_bj_TC_7->value();
    ui->label_baoj_setTC_7->setText(a.setNum(baojing_TC_value[6])+" ℃");
}
void MainWindow::on_verticalSlider_bj_TC_8_valueChanged(int value)
{
    QString a;
    baojing_TC_value[7]=ui->verticalSlider_bj_TC_8->value();
    ui->label_baoj_setTC_8->setText(a.setNum(baojing_TC_value[7])+" ℃");
}

void MainWindow::on_battery_refresh_clicked()
{

    timer_sample->stop();
    ui->start_sample->setEnabled(true);
    unsigned int temp16=0;
    protocol_data_buf[0]=device_ID;
    protocol_data_buf[1]=battery_read_function;
    protocol_data_buf[2]=0x00;
    protocol_data_buf[3]=0x00;
    protocol_data_buf[4]=0x00;
    protocol_data_buf[5]=0x00;
    temp16=CRC_16(protocol_data_buf,6);
    protocol_data_buf[6]= LO_UINT16(temp16);
    protocol_data_buf[7]= HI_UINT16(temp16);
    send_serial_data();
}

void MainWindow::on_strength_refresh_clicked()
{

    timer_sample->stop();
    ui->start_sample->setEnabled(true);
    unsigned int temp16=0;
    protocol_data_buf[0]=device_ID;
    protocol_data_buf[1]=strength_read_fuction;
    protocol_data_buf[2]=0x00;
    protocol_data_buf[3]=0x00;
    protocol_data_buf[4]=0x00;
    protocol_data_buf[5]=0x00;
    temp16=CRC_16(protocol_data_buf,6);
    protocol_data_buf[6]= LO_UINT16(temp16);
    protocol_data_buf[7]= HI_UINT16(temp16);
    send_serial_data();
}

void MainWindow::on_button_readID_clicked()
{
    timer_sample->stop();
    ui->start_sample->setEnabled(true);
    unsigned int temp16=0;
    protocol_data_buf[0]=Address_read_function;
    protocol_data_buf[1]=Address_read_function;
    protocol_data_buf[2]=0x00;
    protocol_data_buf[3]=0x00;
    protocol_data_buf[4]=0x00;
    protocol_data_buf[5]=0x00;
    temp16=CRC_16(protocol_data_buf,6);
    protocol_data_buf[6]= LO_UINT16(temp16);
    protocol_data_buf[7]= HI_UINT16(temp16);
    send_serial_data();

}

void MainWindow::on_button_setID_clicked()
{
    timer_sample->stop();
    ui->start_sample->setEnabled(true);
    unsigned int temp16=0;
    protocol_data_buf[0]=device_ID;
    protocol_data_buf[1]=Address_set_fuction;
    protocol_data_buf[2]=0x00;
    protocol_data_buf[3]=0x00;
    protocol_data_buf[4]=0x00;
    protocol_data_buf[5]=ui->inputBox_id->value();
    temp16=CRC_16(protocol_data_buf,6);
    protocol_data_buf[6]= LO_UINT16(temp16);
    protocol_data_buf[7]= HI_UINT16(temp16);
    send_serial_data();
}

void MainWindow::make_sure()
{
   QMessageBox::information(NULL, tr("提醒"), tr("参数已保存"), QMessageBox::Yes);
}

void MainWindow::on_baoj_save_clicked()
{
    QSettings settings("config.ini", QSettings::IniFormat); // 当前目录的INI文件
    settings.beginGroup("alarm_inf");
    make_sure();
    settings.setValue("ch1_t",baojing_T_value[0]);
    settings.setValue("ch1_tc",baojing_TC_value[0]);
    settings.setValue("ch2_t",baojing_T_value[1]);
    settings.setValue("ch2_tc",baojing_TC_value[1]);
    settings.setValue("ch3_t",baojing_T_value[2]);
    settings.setValue("ch3_tc",baojing_TC_value[2]);
    settings.setValue("ch4_t",baojing_T_value[3]);
    settings.setValue("ch4_tc",baojing_TC_value[3]);
    settings.setValue("ch5_t",baojing_T_value[4]);
    settings.setValue("ch5_tc",baojing_TC_value[4]);
    settings.setValue("ch6_t",baojing_T_value[5]);
    settings.setValue("ch6_tc",baojing_TC_value[5]);
    settings.setValue("ch7_t",baojing_T_value[6]);
    settings.setValue("ch7_tc",baojing_TC_value[6]);
    settings.setValue("ch8_t",baojing_T_value[7]);
    settings.setValue("ch8_tc",baojing_TC_value[7]);
    settings.endGroup();
}



void MainWindow::on_qujian_set_clicked()
{
    //QString str[10]={0};
        //QString str_fan[10]={0};
          str_min = ui->qujian_min->text();//获取qujian_min里面的值
          str_max = ui->qujian_max->text();//获取qujian_max里面的值
         val_min = str_min.toFloat();
         val_max = str_max.toFloat();
         more = (val_max-val_min);
        if(val_min>val_max)
        {
            QMessageBox::information(NULL, tr("警告"), tr("最小温度值不能大于最大温度值"), QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        if(val_min<=val_max)
        {
            //float more = (val_max-val_min);
            //QString str = QString("%1").arg(more);//将float类型转化为QString类型
            //ui->qujian_max->setText(str);//显示获取的值
            for(int i=0;i<10;i++)
               {
                str[i] = QString("%1").arg((more/10)*i+val_min);//使显示左边值
                str_fan[i] = QString("%1").arg((more/10)*i+val_min+(more/10));//使显示右边值
                labels[i] = QString("[")+(str[i])+QString("-")+(str_fan[i])+QString("]");//使显示的形式为[3-3.1]
                }
         }
        make_sure();
        QSettings settings("config.ini", QSettings::IniFormat); // 当前目录的INI文件
        settings.beginGroup("qujian_inf");
        settings.setValue("qujian_min",str_min);
        settings.setValue("qujian_max",str_max);
        settings.endGroup();
}

void MainWindow::on_baoj_save_pre_clicked()
{

}


/*
void MainWindow::WriteSettings()
{
    //QSettings settings("Software Inc", "Spreadsheet"); // windows在注册表中建立建 Software Inc -> Spreadsheet
    QSettings settings("gdfg.ini", QSettings::IniFormat); // 当前目录的INI文件
    settings.beginGroup("DevOption");
    settings.setValue("mainFun", 25);
    settings.setValue("subFun", 40);
    settings.setValue("service", 1);
    settings.endGroup();
}

void MainWindow::ReadSettings()
{
    QSettings settings("Option.ini", QSettings::IniFormat);

    int mainFun = settings.value("DevOption/mainFun").toInt();
    int subFun = settings.value("DevOption/subFun").toInt();
    QString service = settings.value("DevOption/service").toString();

    qDebug() << "service" << service;
    qDebug() << "mainFun" << mainFun;
    qDebug() << "subFun" << subFun;
}
*/
void MainWindow::on_button_save_set_para_clicked()
{
    make_sure();
    QSettings settings("config.ini", QSettings::IniFormat); // 当前目录的INI文件
    settings.beginGroup("device_inf");
    settings.setValue("id",device_ID);
    settings.setValue("sample_time",device_sample_time);
    settings.endGroup();
}
/*
//! [2]

void MyInputPanelContext::sendCharacter(QChar character)
{
    QPointer<QWidget> w = focusWidget();

    if (!w)
        return;

    QKeyEvent keyPress(QEvent::KeyPress, character.unicode(), Qt::NoModifier, QString(character));
    QApplication::sendEvent(w, &keyPress);

    if (!w)
        return;

    QKeyEvent keyRelease(QEvent::KeyPress, character.unicode(), Qt::NoModifier, QString());
    QApplication::sendEvent(w, &keyRelease);
}

//! [2]

//! [3]

void MyInputPanelContext::updatePosition()
{
    QWidget *widget = focusWidget();
    if (!widget)
        return;

    QRect widgetRect = widget->rect();
    QPoint panelPos = QPoint(widgetRect.left(), widgetRect.bottom() + 2);
    panelPos = widget->mapToGlobal(panelPos);
    inputPanel->move(panelPos);
}

//! [3]
*/
