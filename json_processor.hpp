#ifndef JSON_PROCESSOR_HPP
#define JSON_PROCESSOR_HPP

#include <QString>
#include <QTextEdit>
#include <QTableWidget>
#include <QFile>
#include <QFileInfo>
#include <filesystem>
#include <fstream>
#include "include/nlohmann/json.hpp"
#include "product_struct.hpp"

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
namespace fs = std::filesystem;

class jsonProcessor{
public:
    QString convertVowelsToUppercase(QString& line){
        // Список гласных (русский + английский)
        QString vowels = "aeiouyаеёиоуыэюя";
        for (int i = 0; i < line.length(); ++i) {
            if (vowels.contains(line[i], Qt::CaseInsensitive)) {
                line[i] = line[i].toUpper();
            }
        }
        return line;
    }
    QString jsonUpperVowelsInName(QString& text) {
        try{
        json data = json::parse(text.toStdString());
        for (auto& item : data) {
            if (item.contains("name") && item["name"].is_string()) {
                std::string name = item["name"];
                QString qstr = QString::fromStdString(name);
                convertVowelsToUppercase(qstr);
                item["name"] = qstr.toStdString();
            }
        }
        return QString::fromStdString(data.dump(4));
        } catch(const json::parse_error& e){
            qDebug()<<"JSON parse error"<< e.what();
            return "]";
        }
    }
    bool isJsonExtension(const std::string& path) {
        const std::string ext = ".json";
        if (path.length() >= ext.length()) {
            return path.rfind(ext) == path.length() - ext.length();
        } return false;
    }
    virtual bool isOpen() = 0;
protected:
    json loadFromFile(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Cannot open file:" << filePath;
            return json::array();
        }

        QFileInfo fileInfo(file);
        if (!fileInfo.suffix().compare("json", Qt::CaseInsensitive)){
            qDebug() << "The file on path:" << filePath << " - isn't json";
            return json::array();
        }
        QTextStream stream(&file);
        QString content = stream.readAll();

        try{
            return json::parse(content.toStdString());
        }   catch(const json::parse_error& e){
            qDebug() << "JSON parse error in file:" << filePath <<":"<< e.what();
            return json::array();
        }   catch(const json::exception& e){
            qDebug() << "Unexpected error:"<<e.what();
            return json::array();
        }
    }
};

class jsonOutput: public jsonProcessor
{
    fs::path jsonPath_;
    std::ifstream file_;

public:
    jsonOutput(const QString& filePath)
        : jsonPath_(filePath.toStdString())
        , file_(jsonPath_)
    {
        if (!isOpen()) return;
        if (!json::accept(file_)){
            qDebug() << "The readable file on path:" + jsonPath_.string() + " - isn't json";
            return;
        }
        file_.clear();
        file_.seekg(0);
    }

    void outputtingJsonToATextField(QTextEdit *textEdit){
        ordered_json j;
        if (isOpen() and fs::file_size(jsonPath_) > 0){
            file_>>j;
        } else {
            qDebug() << "Failed to output to TextEdit";
            return;
        }
        textEdit->setText(QString::fromStdString(j.dump(4)));
    }
    void outputtingJsonToATable(QTableWidget *table){
        ordered_json j;
        if (!isOpen() || fs::file_size(jsonPath_) == 0){
            table->setRowCount(0);
            qDebug() << "Failed to output to TableWidget";
            return;
        }

        file_.clear();
        file_.seekg(0);
        file_>>j;

        table->setRowCount(0);
        int row = 0;

        for (const auto& item : j){
            Product currentProduct(item);
            if (!currentProduct.isStillGood()) continue;
            table->insertRow(row);

            int id = item["id"];
            QTableWidgetItem *itemId = new QTableWidgetItem(QString::number(id));
            itemId->setTextAlignment(Qt::AlignCenter);
            table->setItem(row, 0, itemId);

            QString originalName = QString::fromStdString(item["name"]);
            QString modifiedName = convertVowelsToUppercase(originalName);
            QTableWidgetItem *itemName = new QTableWidgetItem(modifiedName);
            table->setItem(row, 1, itemName);

            float price = item["price"];
            QTableWidgetItem *itemPrice = new QTableWidgetItem(QString::number(price, 'f', 2));
            itemPrice->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            table->setItem(row, 2, itemPrice);

            QString expiritionDate = QString::fromStdString(item["expiration_date"]); //!!!
            QTableWidgetItem *itemDate = new QTableWidgetItem(expiritionDate);
            itemDate->setTextAlignment(Qt::AlignCenter);
            table->setItem(row, 3, itemDate);

            row++;
        }
    }

    bool isOpen() override{
        return file_.is_open();
    }
    ~jsonOutput(){file_.close();}
    jsonOutput(const jsonOutput&) = delete;
    jsonOutput& operator=(const jsonOutput&) = delete;

    jsonOutput(jsonOutput&&) = default;
    jsonOutput& operator=(jsonOutput&&) = default;
};

class jsonInput: public jsonProcessor{
    std::ofstream file_;
    fs::path jsonPath_;

public:
    jsonInput(const QString& filePath)
        : jsonPath_(filePath.toStdString())
        , file_()
    {
        fs::path directoryPath = jsonPath_.parent_path();

        if (!directoryPath.empty() && !fs::exists(directoryPath)) {
            qDebug() << "Parent directory does not exist";
            return;
        }
        if (!isJsonExtension(filePath.toStdString())){
            qDebug() << "The mutable file on path:" << jsonPath_.string() << "  -  isn't json";
            return;
        }
        if (!fs::exists(jsonPath_)) {
            qDebug() << "The mutable file is created";
        }
        file_.open(jsonPath_, std::ios::out | std::ios::trunc);
        if (!isOpen()) {
            qDebug() << "Failed to open mutable file: " << jsonPath_.string();
            return;
        }
    }

    void saveToJSON(QString&& jsonStr){
        json data = json::parse(jsonStr.toStdString());
        file_<<data.dump(4);
        file_.flush();
        qDebug()<<"File: "+jsonPath_.string()+" saved successfully";
    }

    bool isOpen() override{
        return file_.is_open();
    }
    jsonInput(const jsonInput&) = delete;
    jsonInput& operator=(const jsonInput&) = delete;

    jsonInput(jsonInput&&) = default;
    jsonInput& operator=(jsonInput&&) = default;
};
#endif // JSON_PROCESSOR_HPP
