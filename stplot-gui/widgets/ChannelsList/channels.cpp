#include "channels.h"
#include "ui_channels.h"
#include <QDebug>
#include <qmath.h>
#include <unistd.h>

Channels::Channels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Channels), iteamDeclarater(this), curentColorSet(0), curentDotStyle(MAX_DEFAOULT_DOT_STYLE)
{
    ui->setupUi(this);
    m_channels = new QVector<VarChannel*>();
    m_channelModel = new ChannelModel(m_channels);
    ui->treeView->setItemDelegate(new ChanaleItemDelegate(this));
//    ui->tableView->setItemDelegateForColumn(3, m_channelModel->makeIteamLineStye(this));
    ui->treeView->setModel(m_channelModel);

//    ui->tableView->setItemDelegateForColumn(1, cbid);

    connect(m_channelModel, &ChannelModel::updateViewport,
            ui->treeView->viewport(), QOverload<>::of(&QWidget::update));

    connect(m_channelModel, SIGNAL(changeEnablePlo(VarChannel*,int,bool)), this, SIGNAL(addingChanaleToPlot(VarChannel*,int,bool)));

    //init colur sequnce
    colorSetSequese.append(QColor(40, 110, 255));
    colorSetSequese.append(QColor(255, 110, 40));
    colorSetSequese.append(QColor(40, 255, 110));
    for(int i = 0 ; i < 18; i++ )
    {
        colorSetSequese.append(QColor(qSin(i*0.3)*128+128, qSin(i*0.6+0.7)*128+128, qSin(i*0.4+0.6)*128+128));
    }
//    colorSetSequese.append(QColor("#99e600"));
//    colorSetSequese.append(QColor("#99cc00"));
//    colorSetSequese.append(QColor("#99b300"));
//    colorSetSequese.append(QColor("#9f991a"));
//    colorSetSequese.append(QColor("#a48033"));
//    colorSetSequese.append(QColor("#a9664d"));
//    colorSetSequese.append(QColor("#ae4d66"));
//    colorSetSequese.append(QColor("#b33380"));
//    colorSetSequese.append(QColor("#a64086"));
//    colorSetSequese.append(QColor("#994d8d"));
//    colorSetSequese.append(QColor("#8d5a93"));
//    colorSetSequese.append(QColor("#806699"));
//    colorSetSequese.append(QColor("#8073a6"));
//    colorSetSequese.append(QColor("#8080b3"));
}

Channels::~Channels()
{
    delete ui;
    for (int i = 0; i < m_channels->size(); ++i) {
        delete m_channels->at(i);
    }
    delete m_channels;
    delete m_channelModel;
}

void Channels::saveSettings(QSettings *settings)
{

}

void Channels::restoreSettings(QSettings *settings)
{

}

QVector<VarChannel *> *Channels::getListChanales() const
{
    return m_channels;
}


void Channels::add_channel(varloc_node_t* node){
    for (int i = 0; i < m_channels->size(); ++i) {
        if (m_channels->at(i)->has_var_node(node)){
            //already have node
            return;
        }
    }
    //set colour from sequence
    m_channels->push_back(new VarChannel(node, colorSetSequese[curentColorSet++], curentDotStyle++));
    //reset sequnce colour set
    if(curentColorSet >= colorSetSequese.size())
        curentColorSet = 0;

    if(curentDotStyle >= MAX_NUMBER_DOT_STYLE)
        curentDotStyle = 1;//1 becouse 0 is none style

    emit m_channelModel->layoutChanged();
}

void Channels::addPlot()
{
    m_channelModel->addPlot();
}

void Channels::deletePlot(int number)
{
    m_channelModel->deletePlot(number);
}

void Channels::setPlotName(int number, QString name)
{
    m_channelModel->setPlotName(number, name);
}

void Channels::on_pushButton_clicked()
{
    int curentElement = ui->treeView->currentIndex().row();

    VarChannel* plotChanale = m_channels->at(curentElement);

    static int sinFreq = 0;
    sinFreq++;


    for(int i = 0; i < 100; i++)
    {
        usleep(1000);
        plotChanale->push_value( qSin(i*1.0*2*M_PI*sinFreq/100.0) );
    }


}

