#include "newchanaledialog.h"
#include "ui_newchanaledialog.h"

NewChanaleDialog::NewChanaleDialog(QWidget *parent, int numerNewChanale) :
    QDialog(parent),
    ui(new Ui::NewChanaleDialog)
{
    ui->setupUi(this);
    ui->lineEdit_name->setText("new_varible_" + QString::number(numerNewChanale));
    ui->lineEdit_addres->setValidator(new QRegExpValidator(QRegExp("0x[0-9,a-f,A-F]{7}[0,4,8,c,C]{1}"), this));
}

NewChanaleDialog::~NewChanaleDialog()
{
    delete ui;
}

QString NewChanaleDialog::getName()
{
    return ui->lineEdit_name->text();
}

uint32_t NewChanaleDialog::getBaseAddres()
{
    return ui->lineEdit_addres->text().leftJustified(10, '0').toUInt(nullptr, 16);
}

varloc_loc_type_e NewChanaleDialog::getType()
{
    if(ui->comboBox_typeVarible->currentIndex() == 2)
        return VARLOC_FLOAT;
    else if(ui->comboBox_typeVarible->currentIndex() == 1)
        return VARLOC_SIGNED;
    return VARLOC_UNSIGNED;
}

uint32_t NewChanaleDialog::getSizeBits()
{
    return ui->spinBox_bitsSize->value();
}

uint32_t NewChanaleDialog::getOffsetBits()
{
    return ui->spinBox_bitsOffset->value();
}

void NewChanaleDialog::on_comboBox_typeVarible_activated(int index)
{
    ui->spinBox_bitsSize->setDisabled(index == 2);
}

