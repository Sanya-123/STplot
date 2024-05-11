#ifndef NEWCHANALEDIALOG_H
#define NEWCHANALEDIALOG_H

#include <QDialog>
#include "varcommon.h"

namespace Ui {
class NewChanaleDialog;
}

class NewChanaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewChanaleDialog(QWidget *parent = nullptr, int numerNewChanale = 0);
    ~NewChanaleDialog();
    QString getName();
    uint32_t getBaseAddres();
    varloc_loc_type_e getType();
    uint32_t getSizeBits();
    uint32_t getOffsetBits();

private slots:
    void on_comboBox_typeVarible_activated(int index);

private:
    Ui::NewChanaleDialog *ui;
};

#endif // NEWCHANALEDIALOG_H
