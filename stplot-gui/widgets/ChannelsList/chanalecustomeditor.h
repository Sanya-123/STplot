#ifndef CHANALECUSTOMEDITOR_H
#define CHANALECUSTOMEDITOR_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ChanaleCustomEditor;
}

class ChanaleCustomEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ChanaleCustomEditor(QStringList chanaleNames, QString name = "", QString oldScript = "", QWidget *parent = nullptr);
    ~ChanaleCustomEditor();

    QString getName();
    QString getScipt();

public slots:
    void setScript(QString script);
    void updateChanaleNames(QStringList chanaleNames);


private slots:
    void iteamCliced(QListWidgetItem* iteam);

private:
    Ui::ChanaleCustomEditor *ui;
};

#endif // CHANALECUSTOMEDITOR_H
