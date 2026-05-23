#include "warehouse_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WarehouseWindow w;
    w.show();
    return a.exec();
    /*
    try{
    QApplication a(argc, argv);
    WarehouseWindow w;
    w.show();
    return a.exec();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown Error:\n";
        return 1;
    }*/
}
