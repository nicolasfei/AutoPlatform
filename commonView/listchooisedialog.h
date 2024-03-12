#ifndef LISTCHOOISEDIALOG_H
#define LISTCHOOISEDIALOG_H

#include <QDialog>

namespace Ui {
class ListChooiseDialog;
}

class ListChooiseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListChooiseDialog(QList<QString> list, QWidget *parent = nullptr);
    ~ListChooiseDialog();

    QList<QString>* getSelectList()
    {
        return this->selectList;
    }

signals:
    void selectResult(QList<QString>* list);

private slots:
    void on_itemSelect(int id, bool select);

    void on_buttonBox_accepted();

private:
    Ui::ListChooiseDialog *ui;
    QList<QString> list;
    QList<QString>* selectList;
};

#endif // LISTCHOOISEDIALOG_H
