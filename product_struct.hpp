#ifndef PRODUCTSTRUCT_HPP
#define PRODUCTSTRUCT_HPP

#include <string>
#include <memory>
#include <QString>
#include <QDateTime>
#include <filesystem>
#include <fstream>
#include <concepts>
#include "include/nlohmann/json.hpp"

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
namespace fs = std::filesystem;
/*
template<typename T>
concept stringCompatible = std::is_same_v<T,std::string>||
                           std::is_same_v <T, QString>;
template<stringCompatible T>
*/

inline long dateToLong(const std::string& date) {
    // date = "0123-56-89"
    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));

    return year * 10000L + month * 100 + day;
    // Результат: 20260407
}

inline long currentTime(){
    // 1. Получение текущей даты и времени
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timeString = currentDateTime.toString("yyyy-MM-dd");
    return dateToLong(timeString.toStdString());
}

struct Product {
    Product(std::string category, const std::string& name, float price, long expiration_date, int id, int value)
        : category_(category), name_(name), price_(price), expiration_date_(expiration_date), id_(id), value_(value){}
    Product(const json& jsonObj){
        // Правильный порядок
        id_       =        jsonObj.value("id", 0);
        name_     =        jsonObj.value("name", "");
        value_    =        jsonObj.value("value", 0);
        price_    =        jsonObj.value("price", 0);
        category_ =        jsonObj.value("category", "");
        expiration_date_ = dateToLong(jsonObj.value("expiration_date", ""));
    }
    bool isStillGood(){
        if (currentTime() < expiration_date_) return true;
        else return false;}
    // Порядок для лучшего хранения
    std::string category_;
    std::string name_;
    float price_;
    long expiration_date_;
    int id_;
    int value_;
};

inline std::unique_ptr<Product> createProductFromJson(const json& jsonObj) {
        // Правильный порядок
        int id =                  jsonObj.value("id", 0);
        std::string name =        jsonObj.value("name", "");
        int value =               jsonObj.value("value", 0);
        float price =             jsonObj.value("price", 0);
        std::string category =    jsonObj.value("category", "");
        long expDate = dateToLong(jsonObj.value("expiration_date", ""));

        return std::make_unique<Product>(category, name, price, expDate, id, value);
}

#endif // PRODUCTSTRUCT_H
