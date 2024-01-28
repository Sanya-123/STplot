#include "channels.h"
#include "ui_channels.h"
#include <QDebug>
#include <QMessageBox>
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

    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), m_channelModel, SLOT(selectChanale(QModelIndex)));

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
    settings->beginWriteArray("chanales");
    for (int i = 0; i < m_channels->size(); i++)
    {
        settings->setArrayIndex(i);
        settings->setValue("chanaleName", m_channels->at(i)->getName());
        settings->setValue("displayName", m_channels->at(i)->displayName());
        settings->setValue("dotStyle", m_channels->at(i)->dotStyle());
        settings->setValue("lineStyle", m_channels->at(i)->lineStyle());
        settings->setValue("lineColor", m_channels->at(i)->lineColor());
        settings->setValue("lineWidth", m_channels->at(i)->lineWidth());

        settings->beginGroup("location");
        varloc_location_t loc = m_channels->at(i)->getLocation();
        settings->setValue("type", (int)loc.type);
        settings->setValue("base", loc.address.base);
        settings->setValue("offset_bits", loc.address.offset_bits);
        settings->setValue("size_bits", loc.address.size_bits);
        settings->endGroup();


        settings->setValue("totalSizePlot", m_channels->at(i)->getTotalSizePlot());
        settings->beginWriteArray("plotList");
        QVector<bool> listPlot = m_channels->at(i)->getPlotList();
        for(int j = 0; j < listPlot.size(); j++)
        {
            settings->setArrayIndex(j);
            settings->setValue("enPlot", listPlot[j]);
        }
        settings->endArray();

    }
    settings->endArray();
}

void Channels::restoreSettings(QSettings *settings)
{
    //eset states
    for (int i = 0; i < m_channels->size(); ++i) {
        delete m_channels->at(i);;
    }
    m_channels->clear();

    curentColorSet = 0;
    curentDotStyle = 0;
    emit m_channelModel->layoutChanged();

    int size = settings->beginReadArray("chanales");
    for (int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        //read all setings
        QString chanaleName = settings->value("chanaleName").toString();
        QString displayName = settings->value("displayName").toString();
        int dotStyle = settings->value("dotStyle").toUInt();
        int lineStyle = settings->value("lineStyle").toUInt();
        QColor lineColor = settings->value("lineColor").value<QColor>();
        int lineWidth = settings->value("lineWidth").toUInt();

        settings->beginGroup("location");
        varloc_location_t loc;
        loc.type = (varloc_loc_type_e)settings->value("type").toUInt();
        loc.address.base = settings->value("base").toUInt();
        loc.address.offset_bits = settings->value("offset_bits").toUInt();
        loc.address.size_bits = settings->value("size_bits").toUInt();
        settings->endGroup();

        int totalSizePlot = settings->value("totalSizePlot").toUInt();

        int sizePlotList = settings->beginReadArray("plotList");
        QVector<bool> listPlot;
        for(int j = 0; j < sizePlotList; j++)
        {
            settings->setArrayIndex(j);
            listPlot.append(settings->value("enPlot").toBool());
//            settings->setValue("enPlot", listPlot[j]);
        }
        settings->endArray();

        //now create var
        VarChannel *chanale = new VarChannel(loc, chanaleName, lineColor, dotStyle);
        chanale->setDisplayName(displayName);
        chanale->setLineStyle(lineStyle);
        chanale->setLineWidth(lineWidth);
        chanale->setTotalSizePlot(totalSizePlot);
        m_channels->push_back(chanale);
        for(int j = 0; j < sizePlotList; j++)
        {//TODO add signal to ViewManager
            if(listPlot[j])
            {
//                chanale->setEnableOnPlot(j, listPlot[j]);
                m_channelModel->setEnablePlot(chanale, j, listPlot[j]);
            }
           qDebug() << "plotLisi:" << displayName << j << listPlot[j];
        }
    }
    settings->endArray();

    //update next elements
    curentColorSet = m_channels->size()%colorSetSequese.size();
    curentDotStyle = m_channels->size()%(MAX_NUMBER_DOT_STYLE - 1);

    //if I remove some meadle element an so color is offsetings So I check colors ans set next
    if(m_channels->size())
    {
        //color
        int indexColor = colorSetSequese.indexOf(m_channels->last()->lineColor());
        if(indexColor >= 0)
        {
            curentColorSet = indexColor + 1;
            if(curentColorSet >= colorSetSequese.size())
                curentColorSet = 0;
        }

//        line style
        curentDotStyle = m_channels->last()->dotStyle() + 1;

        if(curentDotStyle >= MAX_NUMBER_DOT_STYLE)
            curentDotStyle = 1;//1 becouse 0 is none style
    }

    emit m_channelModel->layoutChanged();
//    ui->treeView->update();
}

QVector<VarChannel *> *Channels::getListChanales() const
{
    return m_channels;
}

void Channels::reloadChannels(varloc_node_t* root){

    bool update_allowed = false;
    for (int i = 0; i < m_channels->size(); ++i) {
        varloc_node_t* node = var_node_get_by_name(root, m_channels->at(i)->getName().toLocal8Bit().data());
        if (node){
            varloc_location_t new_load_loc = var_node_get_load_location(node);
            qDebug("found node %s %x", node->name, new_load_loc.address.base);
            if (!m_channels->at(i)->hasLocation(new_load_loc)){
                if (!update_allowed){
                    QMessageBox msgBox;
                    msgBox.setText("Addresses of variables changed!");
                    msgBox.setInformativeText("Do you want to update all channels?");
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::Yes);
                    int ret = msgBox.exec();
                    if (ret == QMessageBox::Yes){
                        m_channels->at(i)->setLocation(new_load_loc);
                        update_allowed = true;
                    }
                    else{
                        break;
                    }
                }
                else{
                    m_channels->at(i)->setLocation(new_load_loc);
                }
            }
        }
    }
}


void Channels::add_channel(varloc_node_t* node){

    varloc_location_t load_loc = var_node_get_load_location(node);
    for (int i = 0; i < m_channels->size(); ++i) {
        if (m_channels->at(i)->hasLocation(load_loc)){
            //already have node
            qDebug("Error: %s Channel already exists.", node->name);
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

//void Channels::on_pushButton_clicked()
//{
//    int curentElement = ui->treeView->currentIndex().row();

//    VarChannel* plotChanale = m_channels->at(curentElement);

//    static int sinFreq = 0;
//    sinFreq++;


//    for(int i = 0; i < 100; i++)
//    {
//        usleep(1000);
//        plotChanale->pushValue( qSin(i*1.0*2*M_PI*sinFreq/100.0) );
//    }
//}

void Channels::on_pushButton_deleteChanale_clicked()
{
    int curentElement = ui->treeView->currentIndex().row();
    if((curentElement >= 0) && (curentElement < m_channels->size()))
    {
        m_channels->remove(curentElement);
        emit m_channelModel->layoutChanged();
    }
}

