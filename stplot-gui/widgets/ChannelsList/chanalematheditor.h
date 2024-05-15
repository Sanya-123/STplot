#ifndef CHANALEMATHEDITOR_H
#define CHANALEMATHEDITOR_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ChanaleMathEditor;
}

class ChanaleMathEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ChanaleMathEditor(QStringList chanaleNames, QString name = "", QString oldScript = "", QWidget *parent = nullptr);
    ~ChanaleMathEditor();

    QString getName();
    QString getScipt();

public slots:
    void setScript(QString script);
    void updateChanaleNames(QStringList chanaleNames);


private slots:
    void iteamCliced(QListWidgetItem* iteam);

private:
    Ui::ChanaleMathEditor *ui;
};

#endif // CHANALEMATHEDITOR_H
