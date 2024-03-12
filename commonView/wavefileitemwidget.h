#ifndef WAVEFILEITEMWIDGET_H
#define WAVEFILEITEMWIDGET_H

#include <QWidget>

namespace Ui {
class WaveFileItemWidget;
}

class WaveFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveFileItemWidget(int id, QString name, QWidget *parent = nullptr);
    ~WaveFileItemWidget();

signals:
    void itemSelect(int id, bool select);

private slots:
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::WaveFileItemWidget *ui;
    int id;
    QString name;
};

#endif // WAVEFILEITEMWIDGET_H
