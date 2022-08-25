#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>


namespace Ui {
class OptionsWindow;
}

class OptionsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsWindow(QWidget *parent = nullptr);
    ~OptionsWindow();

private slots:
    void drop_table();

private:
    Ui::OptionsWindow *ui;
};

#endif // OPTIONSWINDOW_H
