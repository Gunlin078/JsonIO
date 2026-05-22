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
    /*
    virtual void output(const QString& path, QTextEdit *textEdit){
        Q_UNUSED(path); Q_UNUSED(textEdit);
        qDebug()<< "Base output was called called";
    }*/
    bool isJsonExtension(const std::string& path) {
        const std::string ext = ".json";
        if (path.length() >= ext.length()) {
            return path.rfind(ext) == path.length() - ext.length();
        } return false;
    }
    void saveToJSON(json& data, const QString& filePath){
        if (!isJsonExtension(filePath.toStdString())){
            qDebug() << "ERROR: The file extension is not json";
            return;
        }
        fs::path jsonPath = filePath.toStdString();
        fs::path directoryPath = jsonPath.parent_path();
        if (!directoryPath.empty() && !fs::exists(directoryPath)) {
            qDebug() << "ERROR: Parent directory does not exist";
            return;
        }
        if (!fs::exists(jsonPath)) {
            qDebug() << "The file created";
            std::ofstream file(jsonPath);
            file.close();}
        std::ofstream o(jsonPath);
        if (o.is_open()){
            o<<data.dump(4);
            o.close();}
        qDebug()<<"File: "+filePath+" saved successfully";
    }
    void outputtingJsonToATable(const QString& filePath, QTableWidget *table){
        json data = loadFromFile(filePath);
        table->setRowCount(0);
        int row = 0;

        for (const auto& item : data){
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
    void outputtingJsonToATextField(const QString& path, QTextEdit *textEdit){
        fs::path jsonPath = path.toStdString();
        if (!fs::exists(jsonPath)) {
            qDebug() << "The file doesn't exist";
            return;}
        ordered_json j;
        std::ifstream i(jsonPath);
        if (i.is_open() and fs::file_size(jsonPath) > 0){
            i>>j;
            i.close();
        }
        textEdit->setText(QString::fromStdString(j.dump(4)));
    }
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
private:
    json loadFromFile(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Cannot open file:" << filePath;
            return json::array();
        }

        QFileInfo fileInfo(file);
        if (!fileInfo.suffix().compare("json", Qt::CaseInsensitive)){
            qDebug() << "The file on path:" << filePath << "isn't json";
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
    std::ifstream file_;
    fs::path jsonPath_;

public:
    jsonOutput(const QString& filePath):jsonPath_(filePath.toStdString())
    {
        std::ifstream file_(jsonPath_);
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open file: " + jsonPath_.string());
        }
        if (!json::accept(file_)){
            throw std::runtime_error( "The file on path:" + jsonPath_.string() + "isn't json");
        }
        file_.clear();
        file_.seekg(0);
    }

    void outputtingJsonToATextField__(QTextEdit *textEdit){
        ordered_json j;
        if (file_.is_open() and fs::file_size(jsonPath_) > 0){
            file_>>j;
        }
        textEdit->setText(QString::fromStdString(j.dump(4)));
    }
    void outputtingJsonToATable__(QTableWidget *table){
        json j;
        if (!file_.is_open() || fs::file_size(jsonPath_) == 0){
            table->setRowCount(0);
            throw std::runtime_error( "The file on path:" + jsonPath_.string() + " - is empty or closed");
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
            throw std::runtime_error( "Parent directory does not exist");
        }
        if (!isJsonExtension(filePath.toStdString())){
            throw std::runtime_error( "The file on path:" + jsonPath_.string() + "isn't json");
        }
        if (!fs::exists(jsonPath_)) qDebug() << "The file is created";

        file_.open(jsonPath_, std::ios::out | std::ios::trunc);
        if (!file_.is_open()) {
            throw std::runtime_error( "Failed to open file: " + jsonPath_.string());
        }
    }

    void saveToJSON__(json& data){
        file_<<data.dump(4);
        file_.flush();
        qDebug()<<"File: "+jsonPath_.string()+" saved successfully";
    }

    jsonInput(const jsonInput&) = delete;
    jsonInput& operator=(const jsonInput&) = delete;

    jsonInput(jsonInput&&) = default;
    jsonInput& operator=(jsonInput&&) = default;
};

#endif // JSON_PROCESSOR_HPP
