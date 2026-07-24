#include "warehouse_window.h"
#include "./ui_warehouse_window.h"

WarehouseWindow::WarehouseWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WarehouseWindow)
{
    ui->setupUi(this);

    if(ui->Output_TableW){
        ui->Output_TableW->setColumnCount(4);
        QStringList headers = {"ID", "Name (Mod)", "Price", "Expiration Date"};
        ui->Output_TableW->setHorizontalHeaderLabels(headers);

        ui->Output_TableW->setColumnWidth(0, 30);
        ui->Output_TableW->setColumnWidth(1, 180);
        ui->Output_TableW->verticalHeader()->setVisible(false);

        ui->Output_TableW->horizontalHeader()->setStretchLastSection(true);
        ui->Output_TableW->setAlternatingRowColors(true);
        ui->Output_TableW->setEditTriggers(QAbstractItemView::NoEditTriggers);}

    QObject::connect(ui->Parse_PB, &QPushButton::clicked, [this](){
        QString userPath = ui->GettingPath_LE->text();
        jsonOutput outputer(userPath);

        outputer.outputtingJsonToATextField(ui->Output_TE);
        outputer.outputtingJsonToATable    (ui->Output_TableW);
        if (ui->Input_TE->toPlainText().isEmpty())
        {
            QString text = ui->Output_TE->toPlainText();
            QString result = outputer.jsonUpperVowelsInName(text);
            ui->Input_TE->setText(result);}});

    QObject::connect(ui->Load_PB,  &QPushButton::clicked, [this](){
        if (ui->Input_TE->toPlainText().isEmpty())       return;
        jsonInput inputer(ui->WritingPath_LE->text());
        inputer.saveToJSON(ui->Input_TE->toPlainText());

    });
}

WarehouseWindow::~WarehouseWindow()
{
    delete ui;
}
