#ifndef JSONOPERATORWINDOW_H
#define JSONOPERATORWINDOW_H

#include <QMainWindow>
#include <QString>
#include <iostream>

#include "jsonProcessor.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class JsonOperatorWindow;
}
QT_END_NAMESPACE

class JsonOperatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    JsonOperatorWindow(QWidget *parent = nullptr);
    //jsonProcessor processor;
    ~JsonOperatorWindow();

private:
    Ui::JsonOperatorWindow *ui;
};
#endif // JSONOPERATORWINDOW_H
