#ifndef WAREHOUSE_WINDOW_H
#define WAREHOUSE_WINDOW_H

#include <QMainWindow>
#include <QString>
#include <iostream>

#include "product_struct.hpp"
#include "json_processor.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class WarehouseWindow;
}
QT_END_NAMESPACE

class WarehouseWindow : public QMainWindow
{
    Q_OBJECT

public:
    WarehouseWindow(QWidget *parent = nullptr);
    //jsonOutput outputer;
    jsonProcessor processor;
    ~WarehouseWindow();

private:
    Ui::WarehouseWindow *ui;
};
#endif // WAREHOUSE_WINDOW_H
