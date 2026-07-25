#include "JsonOperatorWindow.h"
#include "./ui_JsonOperatorWindow.h"
//Потом добавить ctrl+f

JsonOperatorWindow::JsonOperatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JsonOperatorWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->Parse_PB, &QPushButton::clicked, [this](){
        QString userPath = ui->GettingPath_LE->text();
        jsonOutput outputer(userPath);

        outputer.outputtingJsonToATextField(ui->Output_TE);
        if (ui->Input_TE->toPlainText().isEmpty())
        {
            ui->Input_TE->setText(ui->Output_TE->toPlainText());}});

    QObject::connect(ui->Load_PB,  &QPushButton::clicked, [this](){
        if (ui->Input_TE->toPlainText().isEmpty())       return;
        jsonInput inputer(ui->WritingPath_LE->text());
        inputer.saveToJSON(ui->Input_TE->toPlainText());

    });
}

JsonOperatorWindow::~JsonOperatorWindow()
{
    delete ui;
}
