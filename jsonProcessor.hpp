#ifndef JSONPROCESSOR_H
#define JSONPROCESSOR_H

#include <QString>
#include <QTextEdit>
#include <QTableWidget>
#include <QFile>
#include <QFileInfo>
#include <filesystem>
#include <fstream>
#include "include/nlohmann/json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
namespace fs = std::filesystem;

class jsonProcessor{
public:
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
#endif // JSONPROCESSOR_H
