#include "channels.h"
#include "ui_channels.h"

Channels::Channels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Channels)
{
    ui->setupUi(this);
    m_channels = new QVector<VarChannel*>();
    m_channelModel = new ChannelModel(m_channels);
    ui->tableView->setModel(m_channelModel);
    ui->tableView->show();
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


void Channels::add_channel(varloc_node_t* node){
    for (int i = 0; i < m_channels->size(); ++i) {
        if (m_channels->at(i)->has_var_node(node)){
            //already have node
            return;
        }
    }
    m_channels->push_back(new VarChannel(node));
    emit m_channelModel->layoutChanged();
}
