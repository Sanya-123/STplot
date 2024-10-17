#include "channels.h"
#include "ui_channels.h"
#include <QDebug>
#include <QMessageBox>
#include <qmath.h>
#include <unistd.h>
#include "chanalematheditor.h"
#include <QMenu>
#include "newchanaledialog.h"
#include <QTimer>

Channels::Channels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Channels), /*iteamDeclarater(this),*/ curentColorSet(0), curentDotStyle(MAX_DEFAOULT_DOT_STYLE)
{
    ui->setupUi(this);
    m_channels = new QVector<VarChannel*>();
    m_channelsMath = new QVector<VarChannel*>();
    chanaleProxyModel = new QSortFilterProxyModel(this);
    chanaleMathProxyModel = new QSortFilterProxyModel(this);
    m_channelModel = new ChannelModel(m_channels, false, this);
    m_channelMathModel = new ChannelModel(m_channelsMath, true, this);

    //init chanales struct
    chanaleView[ChanalesView].treeView = ui->treeView;
    chanaleView[ChanalesView].channels = m_channels;
    chanaleView[ChanalesView].channelModel = m_channelModel;
    chanaleView[ChanalesView].chanaleProxyModel = chanaleProxyModel;
    chanaleView[MathChanalesView].treeView = ui->treeView_mathChanale;
    chanaleView[MathChanalesView].channels = m_channelsMath;
    chanaleView[MathChanalesView].channelModel = m_channelMathModel;
    chanaleView[MathChanalesView].chanaleProxyModel = chanaleMathProxyModel;

    ui->treeView->setItemDelegate(new ChanaleItemDelegate(false, this));
    ui->treeView_mathChanale->setItemDelegate(new ChanaleItemDelegate(true, this, m_channels));
//    ui->tableView->setItemDelegateForColumn(3, m_channelModel->makeIteamLineStye(this));

    for(int i = 0; i < TotalSizeChanalesView; i++)
    {
        chanaleView[i].chanaleProxyModel->setSourceModel(chanaleView[i].channelModel);
        chanaleView[i].treeView->setModel(chanaleView[i].chanaleProxyModel);

        connect(chanaleView[i].channelModel, &ChannelModel::updateViewport,
                chanaleView[i].treeView->viewport(), QOverload<>::of(&QWidget::update));

        connect(chanaleView[i].channelModel, SIGNAL(changeEnablePlo(VarChannel*,int,bool)), this, SIGNAL(addingChanaleToPlot(VarChannel*,int,bool)));

        connect(chanaleView[i].treeView, SIGNAL(clicked(QModelIndex)), chanaleView[i].channelModel, SLOT(selectChanale(QModelIndex)));

        chanaleView[i].treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

        chanaleView[i].treeView->setContextMenuPolicy(Qt::CustomContextMenu);

        connect(chanaleView[i].treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(openChanaleMenu(QPoint)));

        //set eneble sort by first click
        connect(chanaleView[i].treeView->header(), &QHeaderView::sectionClicked, this, [=](int iteraration)
        {
            chanaleView[i].treeView->header()->setSectionsClickable(false);
            chanaleView[i].treeView->setSortingEnabled(true);
            disconnect(chanaleView[i].treeView->header(), SIGNAL(sectionClicked(int)));
        });
        chanaleView[i].treeView->header()->setSectionsClickable(true);
    }

    connect(ui->lineEdit_filterChanale, SIGNAL(textChanged(QString)), chanaleProxyModel, SLOT(setFilterFixedString(QString)));
    connect(ui->lineEdit_filterMathChanale, SIGNAL(textChanged(QString)), chanaleMathProxyModel, SLOT(setFilterFixedString(QString)));

//    //set eneble sort by first click
//    connect(ui->treeView->header(), &QHeaderView::sectionClicked, this, [=](int iteraration)
//    {
//        ui->treeView->header()->setSectionsClickable(false);
//        ui->treeView->setSortingEnabled(true);
//        disconnect(ui->treeView->header(), SIGNAL(sectionClicked(int)));
//    });
//    ui->treeView->header()->setSectionsClickable(true);
//    connect(ui->treeView_mathChanale->header(), &QHeaderView::sectionClicked, this, [=](int iteraration)
//    {
//        ui->treeView_mathChanale->header()->setSectionsClickable(false);
//        ui->treeView_mathChanale->setSortingEnabled(true);
//        disconnect(ui->treeView_mathChanale->header(), SIGNAL(sectionClicked(int)));
//    });
//    ui->treeView_mathChanale->header()->setSectionsClickable(true);


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
    delete chanaleProxyModel;
    delete chanaleMathProxyModel;
}

void Channels::saveSettings(QSettings *settings)
{
    settings->setValue("splitteChanales", ui->splitter->saveState());
    settings->setValue("splitteChanalesChanales", ui->splitter_chanale->saveState());
    settings->setValue("splitteChanalesMathChanales", ui->splitter_mathChanale->saveState());
    settings->beginWriteArray("chanales");
    for (int i = 0; i < m_channels->size(); i++)
    {
        settings->setArrayIndex(i);
        saveSettingsChanaleCommon(settings, m_channels->at(i));

        saveSettingsChanaleLocation(settings, m_channels->at(i));

        saveSettingsChanalePlot(settings, m_channels->at(i));
    }
    settings->endArray();

    settings->beginWriteArray("chanalesMath");
    for (int i = 0; i < m_channelsMath->size(); i++)
    {
        settings->setArrayIndex(i);
        saveSettingsChanaleCommon(settings, m_channelsMath->at(i));

        saveSettingsChanaleScript(settings, m_channelsMath->at(i));

        saveSettingsChanalePlot(settings, m_channelsMath->at(i));
    }
    settings->endArray();
}

void Channels::restoreSettings(QSettings *settings)
{
    ui->splitter->restoreState(settings->value("splitteChanales").toByteArray());
    ui->splitter_chanale->restoreState(settings->value("splitteChanalesChanales").toByteArray());
    ui->splitter_mathChanale->restoreState(settings->value("splitteChanalesMathChanales").toByteArray());

    //reset states
    for (int i = 0; i < m_channels->size(); ++i) {
        disconnect(m_channels->at(i), SIGNAL(requestWriteData(uint64_t,varloc_location_t)), this, SIGNAL(requestWriteData(uint64_t,varloc_location_t)));
        for(int j = 0; j < m_channels->at(i)->getTotalSizePlot(); j++)
        {
            if(m_channels->at(i)->isEnableOnPlot(j))
            {
                m_channels->at(i)->setEnableOnPlot(j, false);
                emit addingChanaleToPlot(m_channels->at(i), j, false);
            }
        }
        delete m_channels->at(i);
    }
    m_channels->clear();

    for (int i = 0; i < m_channelsMath->size(); ++i) {
        for(int j = 0; j < m_channelsMath->at(i)->getTotalSizePlot(); j++)
        {
            if(m_channelsMath->at(i)->isEnableOnPlot(j))
            {
                m_channelsMath->at(i)->setEnableOnPlot(j, false);
                emit addingChanaleToPlot(m_channelsMath->at(i), j, false);
            }
        }
        delete m_channelsMath->at(i);
    }
    m_channelsMath->clear();

    curentColorSet = 0;
    curentDotStyle = 0;
    chanaleProxyModel->setSourceModel(nullptr);
    chanaleMathProxyModel->setSourceModel(nullptr);
    chanaleProxyModel->setSourceModel(m_channelModel);
    chanaleMathProxyModel->setSourceModel(m_channelMathModel);
    emit m_channelModel->layoutChanged();
    emit m_channelMathModel->layoutChanged();

    //common stuff for chanales
    QString chanaleName;
    QString displayName;
    int dotStyle;
    int lineStyle;
    QColor lineColor;
    int lineWidth;
    double offset, mult;

    int size = settings->beginReadArray("chanales");
    for (int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        //read all setings

        restoreSettingsChanaleCommon(settings, &chanaleName, &displayName, &dotStyle,
                                     &lineStyle, &lineColor, &lineWidth,
                                     &offset, &mult);

        varloc_location_t loc = restoreSettingsChanaleLocation(settings);

        int totalSizePlot = settings->value("totalSizePlot").toUInt();

        QVector<bool> listPlot = restoreSettingsChanaleListPlot(settings);

        //now create var
        VarChannel *chanale = new VarChannel(loc, chanaleName, lineColor, dotStyle);
        chanale->setDisplayName(displayName);
        chanale->setLineStyle(lineStyle);
        chanale->setLineWidth(lineWidth);
        chanale->setTotalSizePlot(totalSizePlot);
        chanale->setOffset(offset);
        chanale->setMult(mult);
        m_channels->push_back(chanale);
        connect(chanale, SIGNAL(requestWriteData(uint64_t,varloc_location_t)), this, SIGNAL(requestWriteData(uint64_t,varloc_location_t)));
        for(int j = 0; j < listPlot.size(); j++)
        {
            if(listPlot[j])
            {
                m_channelModel->setEnablePlot(chanale, j, listPlot[j]);
            }
        }
    }
    settings->endArray();

    size = settings->beginReadArray("chanalesMath");
    for (int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        //read all setings
        restoreSettingsChanaleCommon(settings, &chanaleName, &displayName, &dotStyle,
                                     &lineStyle, &lineColor, &lineWidth,
                                     &offset, &mult);

        int totalSizePlot = settings->value("totalSizePlot").toUInt();

        QVector<bool> listPlot = restoreSettingsChanaleListPlot(settings);

        QString script = restoreSettingsChanaleScript(settings);

        //now create var
        VarChannel *chanale = new VarChannel(script, chanaleName, lineColor, dotStyle);
        chanale->setDisplayName(displayName);
        chanale->setLineStyle(lineStyle);
        chanale->setLineWidth(lineWidth);
        chanale->setTotalSizePlot(totalSizePlot);
        chanale->setOffset(offset);
        chanale->setMult(mult);
        m_channelsMath->push_back(chanale);
        for(int j = 0; j < listPlot.size(); j++)
        {
            if(listPlot[j])
            {
                m_channelMathModel->setEnablePlot(chanale, j, listPlot[j]);
            }
        }
    }
    settings->endArray();

    //update next elements
    curentColorSet = (m_channels->size() + m_channelsMath->size())%colorSetSequese.size();
    curentDotStyle = (m_channels->size() + m_channelsMath->size())%(MAX_NUMBER_DOT_STYLE - 1);

    //if I remove some meadle element an so color is offsetings So I check colors ans set next
    if(m_channels->size() || m_channelsMath->size())
    {
        //color
        int indexColor = colorSetSequese.indexOf(/*m_channels->last()->lineColor() */lineColor);
        if(indexColor >= 0)
        {
            curentColorSet = indexColor + 1;
            if(curentColorSet >= colorSetSequese.size())
                curentColorSet = 0;
        }

//        line style
        curentDotStyle = /*m_channels->last()->dotStyle()*/dotStyle + 1;

        if(curentDotStyle >= MAX_NUMBER_DOT_STYLE)
            curentDotStyle = 1;//1 becouse 0 is none style
    }

    chanaleProxyModel->setSourceModel(nullptr);
    chanaleMathProxyModel->setSourceModel(nullptr);
    chanaleProxyModel->setSourceModel(m_channelModel);
    chanaleMathProxyModel->setSourceModel(m_channelMathModel);
    emit m_channelModel->layoutChanged();
    emit m_channelMathModel->layoutChanged();
//    ui->treeView->update();
}

QVector<VarChannel *> *Channels::getListChanales() const
{
    return m_channels;
}

QVector<VarChannel *> *Channels::getListMathChanales() const
{
    return m_channelsMath;
}

void Channels::reloadChannels(varloc_node_t* root){

    bool update_allowed = false;
    bool not_found = false;
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
        else{
            m_channels->at(i)->clearLocation();
            not_found = true;
        }
    }
    if (not_found){
        QMessageBox msgBox;
        msgBox.setText("Some variables could not be found in elf file!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
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
    connect(m_channels->last(), SIGNAL(requestWriteData(uint64_t,varloc_location_t)), this, SIGNAL(requestWriteData(uint64_t,varloc_location_t)));
    //reset sequnce colour set
    if(curentColorSet >= colorSetSequese.size())
        curentColorSet = 0;

    if(curentDotStyle >= MAX_NUMBER_DOT_STYLE)
        curentDotStyle = 1;//1 becouse 0 is none style

    chanaleProxyModel->setSourceModel(nullptr);
    chanaleProxyModel->setSourceModel(m_channelModel);
    emit m_channelModel->layoutChanged();
}

void Channels::addPlot()
{
    for(int i = 0 ; i < TotalSizeChanalesView; i++){
        chanaleView[i].treeView->clearSelection();
        chanaleView[i].channelModel->addPlot();
    }
}

void Channels::deletePlot(int number)
{
    for(int i = 0 ; i < TotalSizeChanalesView; i++)
        chanaleView[i].channelModel->deletePlot(number);
}

void Channels::setPlotName(int number, QString name)
{
    for(int i = 0 ; i < TotalSizeChanalesView; i++)
        chanaleView[i].channelModel->setPlotName(number, name);
}

void Channels::clearAllChanales()
{
    for(int i = 0; i < m_channels->size(); i++)
        m_channels->at(i)->requestClearBufer();
    for(int i = 0; i < m_channelsMath->size(); i++)
        m_channelsMath->at(i)->requestClearBufer();
}

void Channels::saveSettingsChanaleCommon(QSettings *settings, VarChannel *chanale)
{
    settings->setValue("chanaleName", chanale->getName());
    settings->setValue("displayName", chanale->displayName());
    settings->setValue("dotStyle", chanale->dotStyle());
    settings->setValue("lineStyle", chanale->lineStyle());
    settings->setValue("lineColor", chanale->lineColor());
    settings->setValue("lineWidth", chanale->lineWidth());
    settings->setValue("offset", chanale->getOffset());
    settings->setValue("mult", chanale->getMult());
}

void Channels::saveSettingsChanaleLocation(QSettings *settings, VarChannel *chanale)
{
    settings->beginGroup("location");
    varloc_location_t loc = chanale->getLocation();
    settings->setValue("type", (int)loc.type);
    settings->setValue("base", loc.address.base);
    settings->setValue("offset_bits", loc.address.offset_bits);
    settings->setValue("size_bits", loc.address.size_bits);
    settings->setValue("mask", (quint64)loc.mask);
    settings->endGroup();
}

void Channels::saveSettingsChanaleScript(QSettings *settings, VarChannel *chanale)
{
    settings->setValue("script",  chanale->script());
}

void Channels::saveSettingsChanalePlot(QSettings *settings, VarChannel *chanale)
{
    settings->setValue("totalSizePlot", chanale->getTotalSizePlot());
    settings->beginWriteArray("plotList");
    QVector<bool> listPlot = chanale->getPlotList();
    for(int j = 0; j < listPlot.size(); j++)
    {
        settings->setArrayIndex(j);
        settings->setValue("enPlot", listPlot[j]);
    }
    settings->endArray();

}

varloc_location_t Channels::restoreSettingsChanaleLocation(QSettings *settings)
{
    settings->beginGroup("location");
    varloc_location_t loc;
    loc.type = (varloc_loc_type_e)settings->value("type").toUInt();
    loc.address.base = settings->value("base").toUInt();
    loc.address.offset_bits = settings->value("offset_bits").toUInt();
    loc.address.size_bits = settings->value("size_bits").toUInt();
    loc.mask = settings->value("mask", 0).toULongLong();
    settings->endGroup();

    return loc;

}

void Channels::restoreSettingsChanaleCommon(QSettings *settings, QString *chanaleName, QString *displayName, int *dotStyle, int *lineStyle, QColor *lineColor, int *lineWidth, double *offset, double *mult)
{
    *chanaleName = settings->value("chanaleName").toString();
    *displayName = settings->value("displayName").toString();
    *dotStyle = settings->value("dotStyle").toUInt();
    *lineStyle = settings->value("lineStyle").toUInt();
    *lineColor = settings->value("lineColor").value<QColor>();
    *lineWidth = settings->value("lineWidth").toUInt();
    *offset = settings->value("offset", 0.0).toDouble();
    *mult = settings->value("mult", 1.0).toDouble();
}

QString Channels::restoreSettingsChanaleScript(QSettings *settings)
{
    return settings->value("script").toString();
}

QVector<bool> Channels::restoreSettingsChanaleListPlot(QSettings *settings)
{
    int sizePlotList = settings->beginReadArray("plotList");
    QVector<bool> listPlot;
    for(int j = 0; j < sizePlotList; j++)
    {
        settings->setArrayIndex(j);
        listPlot.append(settings->value("enPlot").toBool());
//            settings->setValue("enPlot", listPlot[j]);
    }
    settings->endArray();

    return listPlot;
}

void Channels::deleteSelectedChanales(ListChanaleView numList)
{
    //get selected chanles
    QModelIndexList listSelected = chanaleView[numList].treeView->selectionModel()->selectedRows();
    QVector<VarChannel*> selectedChanales;

    for(int i = 0; i < listSelected.size(); i++)
    {
        //now it is sorter woe so I ned to tor number in sourse element
        int indexRow = chanaleView[numList].chanaleProxyModel->mapToSource(listSelected[i]).row();
        if(indexRow < chanaleView[numList].channels->size())
        {
            selectedChanales.append(chanaleView[numList].channels->at(indexRow));
        }
    }

    for(int i = 0; i < selectedChanales.size(); i++)
    {
        for(int j = 0; j < selectedChanales[i]->getTotalSizePlot(); j++)
        {
            if(selectedChanales[i]->isEnableOnPlot(j))
            {
                selectedChanales[i]->setEnableOnPlot(j, false);
                emit addingChanaleToPlot(selectedChanales[i], j, false);
            }
        }

        disconnect(selectedChanales[i], SIGNAL(requestWriteData(uint64_t,varloc_location_t)), this, SIGNAL(requestWriteData(uint64_t,varloc_location_t)));
        if(chanaleView[numList].channels->indexOf(selectedChanales[i]) >= 0)
            chanaleView[numList].channels->remove(chanaleView[numList].channels->indexOf(selectedChanales[i]));
    }

    chanaleView[numList].chanaleProxyModel->invalidate();
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
    deleteSelectedChanales(ChanalesView);
}

void Channels::on_pushButton_addMathChanale_clicked()
{
    QStringList chanaleNames;
    foreach (VarChannel* chanale, *m_channels) {
        chanaleNames << chanale->displayName();
    }

    ChanaleMathEditor editor(chanaleNames, "", "", this);
    editor.show();
    editor.exec();

    if(editor.result() == QDialog::Accepted)
    {
        QString name = editor.getName();
        QString script = editor.getScipt();

        m_channelsMath->push_back(new VarChannel(script, name, colorSetSequese[curentColorSet++], curentDotStyle++));

        //reset sequnce colour set
        if(curentColorSet >= colorSetSequese.size())
            curentColorSet = 0;

        if(curentDotStyle >= MAX_NUMBER_DOT_STYLE)
            curentDotStyle = 1;//1 becouse 0 is none style

        chanaleMathProxyModel->invalidate();
    }
}

void Channels::on_pushButton_deleteMathChanale_clicked()
{
    deleteSelectedChanales(MathChanalesView);
}

void Channels::openChanaleMenu(const QPoint &point)
{
    QTreeView *treeView = qobject_cast<QTreeView*>(sender());
//    ChannelModel *modele = nullptr;
//    QVector<VarChannel*> *chanales = nullptr;
    ListChanaleView numberChanaleView = TotalSizeChanalesView;
    if(treeView == ui->treeView)
        numberChanaleView = ChanalesView;
    else if(treeView == ui->treeView_mathChanale)
        numberChanaleView = MathChanalesView;
    else
        return;

    //get selected chanles
    QVector<VarChannel*> selectedChanales;
    QModelIndexList listSelected = treeView->selectionModel()->selectedRows();
    for(int i = 0; i < listSelected.size(); i++)
    {
        if(listSelected[i].row() < chanaleView[numberChanaleView].channels->size())
        {
            selectedChanales.append(chanaleView[numberChanaleView].channels->at(listSelected[i].row()));
        }
    }

    //create menu
    QList<QString> graphNames = chanaleView[numberChanaleView].channelModel->getGraphNames();
    QList<QAction*> listActionsGraph;
    listActionsGraph.append(new QAction("new chanale", this));
    listActionsGraph.append(new QAction("reload chanales", this));
    listActionsGraph.append(new QAction("clear chanales", this));
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    listActionsGraph.append(separator);

    //TODO temporay disable while update math snale is note implemented
    if(numberChanaleView == MathChanalesView)
        listActionsGraph[1]->setEnabled(false);

    int offsetConfigActions = listActionsGraph.size();

    //add eneble on graphs
    for (int i = 0; i < graphNames.size(); i++)
    {
        QString graphName = graphNames[i];
        QAction *action = new QAction(graphName, this);
        action->setCheckable(true);
        bool isEnablOnAllGraphs = true;
        //if graps eneble on all graph set checked true else set false
        foreach (VarChannel *chanale, selectedChanales) {
            if(!chanale->isEnableOnPlot(i))
                isEnablOnAllGraphs = false;
        }
        action->setChecked(isEnablOnAllGraphs);
        listActionsGraph.append(action);
    }


    QAction* res = QMenu::exec(listActionsGraph, treeView->viewport()->mapToGlobal(point), nullptr, this);
    if(res == nullptr)
    {
//        qDebug() << "nothing change";
    }
    else
    {
        if(listActionsGraph.contains(res))
        {
            //set eneble for selected graphs
            int indexSelectGraph = listActionsGraph.indexOf(res);
            if(indexSelectGraph < offsetConfigActions)
            {//add new chanales
                switch(indexSelectGraph)
                {
                    case 0:
                    {
                        if(numberChanaleView == MathChanalesView)
                        {
                            on_pushButton_addMathChanale_clicked();
                        }
                        else
                        {
                            NewChanaleDialog newChanaleDialog(this);
                            if(newChanaleDialog.exec())
                            {
                                QString chanaleName = newChanaleDialog.getName();
                                varloc_location_t loc;
                                loc.type = newChanaleDialog.getType();
                                loc.address.base = newChanaleDialog.getBaseAddres();
                                loc.address.offset_bits = newChanaleDialog.getOffsetBits();
                                loc.address.size_bits = newChanaleDialog.getSizeBits();
                                loc.mask = (((uint64_t)1) << loc.address.size_bits) - 1;
                                loc.mask = loc.mask << loc.address.offset_bits;
                                //set colour from sequence
                                chanaleView[numberChanaleView].channels->push_back(new
                                                                                   VarChannel(loc, chanaleName,
                                                                                              colorSetSequese[curentColorSet++],
                                                                                   curentDotStyle++));
                                connect(chanaleView[numberChanaleView].channels->last(), SIGNAL(requestWriteData(uint64_t,varloc_location_t)),
                                        this, SIGNAL(requestWriteData(uint64_t,varloc_location_t)));
                                //reset sequnce colour set
                                if(curentColorSet >= colorSetSequese.size())
                                    curentColorSet = 0;

                                if(curentDotStyle >= MAX_NUMBER_DOT_STYLE)
                                    curentDotStyle = 1;//1 becouse 0 is none style

                                chanaleView[numberChanaleView].chanaleProxyModel->invalidate();
                                //scrol to element
                                chanaleView[numberChanaleView].treeView->scrollToBottom();
                            }
                        }
                        break;
                    }
                    case 1:
                    {
                        foreach (VarChannel *chanale, selectedChanales) {
                            chanale->requestClearGraph();
                            if(numberChanaleView == MathChanalesView)
                            {
                                //recalculate math chanales
                                //TODO implement recalc math chanales
                            }
                            else
                            {
                                chanale->reloadValues();
                            }

                            emit requestReplot();
                        }

                        break;
                    }
                    case 2:
                        foreach (VarChannel *chanale, selectedChanales)
                            chanale->requestClearBufer();
                        break;
                    default:
                        break;
                }
            }
            else
            {
                //0 element is add gruph
                indexSelectGraph = indexSelectGraph - offsetConfigActions;
                bool isEnableOnGraph = res->isChecked();
//                qDebug() << "indexSelectGraph:" << indexSelectGraph << isEnableOnGraph;
                foreach (VarChannel *chanale, selectedChanales) {
                    chanale->setEnableOnPlot(indexSelectGraph, isEnableOnGraph);
                    emit addingChanaleToPlot(chanale, indexSelectGraph, isEnableOnGraph);
                }
            }
        }
    }

    //delete actions
    foreach (QAction *action, listActionsGraph)
    {
        delete action;
    }
}
