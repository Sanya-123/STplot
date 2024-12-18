#include "channelmodel.h"
#include <QDebug>
#include "qcustomplot.h"

#define GRUPH_FERST_COLUMN          (isMathChanale ? 6 : 8)

ChannelModel::ChannelModel(QVector<VarChannel*> *channels, bool mathChanale, QObject *parent)
    : QAbstractTableModel{parent}, numberGraph(0), isMathChanale(mathChanale)
{
    m_channels = channels;
    dotStyles = getDotStyle();
    lineStyles = getLineStyle();
    lineWidths = getLineWidth();
}

//Qt::ItemFlags ChannelModel::flags(const QModelIndex &index) const{
//    return Qt::ItemFlags();
//}

int ChannelModel::rowCount(const QModelIndex &parent) const
{
    return m_channels->size();
}

int ChannelModel::columnCount(const QModelIndex &parent) const
{
    return GRUPH_FERST_COLUMN + numberGraph;
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0){
            return QVariant( m_channels->at(index.row())->displayName());
        }
        else if(index.column() == 1){
            if(isMathChanale)
                return QVariant(m_channels->at(index.row())->script());
            else
            {
                varloc_location_t loc = m_channels->at(index.row())->getLocation();
                return QVariant("0x" + QString::number(loc.address.base + loc.address.offset_bits/8, 16).rightJustified(8, '0'));
            }
        }
        else if(index.column() == 2){
            return QVariant(m_channels->at(index.row())->lineColor().name());
        }
        else if(index.column() == 3){
            return QVariant(dotStyles[m_channels->at(index.row())->dotStyle()]);
        }
        else if(index.column() == 4){
            return QVariant(lineStyles[m_channels->at(index.row())->lineStyle()]);
        }
        else if(index.column() == 5){
            return QVariant(lineWidths[m_channels->at(index.row())->lineWidth()]);
        }
        else if(index.column() == 6 && !isMathChanale){
            return QVariant(m_channels->at(index.row())->getOffset());
        }
        else if(index.column() == 7 && !isMathChanale){
            return QVariant(m_channels->at(index.row())->getMult());
        }
        else
            return QVariant("");
    }
    else if (role == Qt::CheckStateRole)
    {
        //set column graph as checke
        if(index.column() >= GRUPH_FERST_COLUMN)
        {
            int numGraph = index.column() - GRUPH_FERST_COLUMN;
            if(numGraph >= numberGraph)
                return QVariant();

            //if varible is new you should reinit size gruph
            if(m_channels->at(index.row())->getTotalSizePlot() != numberGraph)
            {
                m_channels->at(index.row())->setTotalSizePlot(0);
                m_channels->at(index.row())->setTotalSizePlot(numberGraph);
            }

            return m_channels->at(index.row())->isEnableOnPlot(index.column() - GRUPH_FERST_COLUMN) ? Qt::Checked : Qt::Unchecked;
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if(index.column() == 2)
            return QVariant(m_channels->at(index.row())->lineColor());
    }


    return QVariant();
}

QVariant ChannelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        if (section == 0){
            return QVariant("Name");
        }
        if (section == 1){
            if(isMathChanale)
                return QVariant("Script");
            else
                return QVariant("Addres");
        }
        else if(section == 2)
            return QVariant("LineColor");
        else if(section == 3)
            return QVariant("DotStyle");
        else if(section == 4)
            return QVariant("LineStyle");
        else if(section == 5)
            return QVariant("LineWidth");
        else if(section == 6 && !isMathChanale)
            return QVariant("offset");
        else if(section == 7 && !isMathChanale)
            return QVariant("mult");
        else if((section - GRUPH_FERST_COLUMN) <  numberGraph)
        {
            return QVariant(graphNames[section - GRUPH_FERST_COLUMN]);
        }
    }
    return QVariant();
}

bool ChannelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.isValid())
    {
        if(index.column() >= GRUPH_FERST_COLUMN)
        {
//            qDebug() << index.column() << index.row() << value;
            setEnablePlot(m_channels->at(index.row()), index.column() - GRUPH_FERST_COLUMN, value.toInt());

            QModelIndex topLeft = index;
            QModelIndex bottomRight = index;
            emit dataChanged(topLeft, bottomRight);

            return true;
        }
    }
    else if(role == Qt::EditRole && index.isValid())
    {
        if(index.column() == 0)
        {
            m_channels->at(index.row())->setDisplayName(value.toString());
            return true;
        }
        if(index.column() == 2)
        {
            m_channels->at(index.row())->setLineColor(QColor(value.toString()));
            return true;
        }
        else if(index.column() == 3)
        {
            m_channels->at(index.row())->setDotStyle(value.toInt());
            return true;
        }
        else if(index.column() == 4)
        {
            m_channels->at(index.row())->setLineStyle(value.toInt());
            return true;
        }
        else if(index.column() == 5)
        {
            m_channels->at(index.row())->setLineWidth(value.toInt());
            return true;
        }
        if((index.column() == 1) && isMathChanale)
        {
            m_channels->at(index.row())->setScript(value.toString());
            return true;
        }
        else if(index.column() == 6 && !isMathChanale)
        {
            m_channels->at(index.row())->setOffset(value.toDouble());
            return true;
        }
        else if(index.column() == 7 && !isMathChanale)
        {
            m_channels->at(index.row())->setMult(value.toDouble());
            return true;
        }
    }

    return QAbstractTableModel::setData(index, value, role);
}

Qt::ItemFlags ChannelModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || index.column() >= (numberGraph + GRUPH_FERST_COLUMN))
        return QAbstractTableModel::flags(index);

    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (index.column() >= GRUPH_FERST_COLUMN)
        return /*flags | */Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else if(index.column() != 1)
        return /*flags | */Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else if((index.column() == 1) && isMathChanale)
        return /*flags | */Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return flags;
}

ComboBoxDelegate *ChannelModel::makeIteamLineStye(QObject *parent)
{

    return new ComboBoxDelegate(getLineStyle(), parent);
}

ComboBoxDelegate *ChannelModel::makeIteamLineWidth(QObject *parent)
{

    return new ComboBoxDelegate(getLineWidth(), parent);
}

ComboBoxDelegate *ChannelModel::makeIteamDotStye(QObject *parent)
{

    return new ComboBoxDelegate(getDotStyle(), parent);
}

QStringList ChannelModel::getDotStyle()
{
    //https://www.qcustomplot.com/index.php/demos/scatterstyledemo
    QVector<QCPScatterStyle::ScatterShape> shapes;
    shapes << QCPScatterStyle::ssNone;
    shapes << QCPScatterStyle::ssDot;
    shapes << QCPScatterStyle::ssCross;
    shapes << QCPScatterStyle::ssPlus;
    shapes << QCPScatterStyle::ssCircle;
    shapes << QCPScatterStyle::ssDisc;
    shapes << QCPScatterStyle::ssSquare;
    shapes << QCPScatterStyle::ssDiamond;
    shapes << QCPScatterStyle::ssStar;
    shapes << QCPScatterStyle::ssTriangle;
    shapes << QCPScatterStyle::ssTriangleInverted;
    shapes << QCPScatterStyle::ssCrossSquare;
    shapes << QCPScatterStyle::ssPlusSquare;
    shapes << QCPScatterStyle::ssCrossCircle;
    shapes << QCPScatterStyle::ssPlusCircle;
    shapes << QCPScatterStyle::ssPeace;

    QStringList dotStyles;

    //get name for each style
    for(int i = 0; i < shapes.size(); i++)
    {
        QString name = QCPScatterStyle::staticMetaObject.enumerator(
                    QCPScatterStyle::staticMetaObject.indexOfEnumerator("ScatterShape")).valueToKey(shapes.at(i));

        dotStyles.append(name);
    }

    return dotStyles;
}

QStringList ChannelModel::getLineWidth()
{
    QStringList options = {"0", "1", "2", "3", "4", "5"};
    return options;
}

QStringList ChannelModel::getLineStyle()
{
    QVector<QCPGraph::LineStyle> shapes;
    shapes << QCPGraph::lsNone;
    shapes << QCPGraph::lsLine;
    shapes << QCPGraph::lsStepLeft;
    shapes << QCPGraph::lsStepRight;
    shapes << QCPGraph::lsStepCenter;
    shapes << QCPGraph::lsImpulse;

    QStringList lineStyles;

    //get name for each style
    for(int i = 0; i < shapes.size(); i++)
    {
        QString name = QCPGraph::staticMetaObject.enumerator(
                    QCPGraph::staticMetaObject.indexOfEnumerator("LineStyle")).valueToKey(shapes.at(i));

        lineStyles.append(name);
    }

    return lineStyles;
}

void ChannelModel::addPlot()
{
    numberGraph++;
    graphNames.append("_");

    for(int i = 0; i < m_channels->size(); i++)
    {
        m_channels->at(i)->setTotalSizePlot(numberGraph);
    }

    emit updateViewport();
    emit layoutChanged();
}

void ChannelModel::deletePlot(int number)
{
    if(number < numberGraph)
    {
        //update grupt list for each varible
        for(int i = 0; i < m_channels->size(); i++)
        {
            if(m_channels->at(i)->getTotalSizePlot() == numberGraph)
            {
                m_channels->at(i)->removePlot(number);
//                plotList->removeAt(number);
            }//if size != numberGraph it is mean that there is error in var list
            else
            {
                m_channels->at(i)->setTotalSizePlot(0);
                m_channels->at(i)->setTotalSizePlot(numberGraph);
            }

        }

        numberGraph--;
        graphNames.removeAt(number);
        emit updateViewport();
        emit layoutChanged();
    }
}

void ChannelModel::setPlotName(int number, QString name)
{
    if(number < numberGraph)
    {
        graphNames[number] = name;
        emit updateViewport();
        emit layoutChanged();
    }
}

void ChannelModel::selectChanale(QModelIndex index)
{
    if(index.isValid())
    {
        if(index.row() < m_channels->size())
        {
            m_channels->at(index.row())->selectCurentPlot();
        }
    }
}

void ChannelModel::setEnablePlot(VarChannel *chanale, int numGruph, bool en)
{
    chanale->setEnableOnPlot(numGruph, en);
    emit changeEnablePlo(chanale, numGruph, en);
}

QList<QString> ChannelModel::getGraphNames() const
{
    return graphNames;
}
