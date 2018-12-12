#include "printer.h"

Printer::Printer(const string name) {
    _name = name;
    HANDLE* handle = new HANDLE();

    if(OpenPrinterA((LPSTR)name.c_str(), handle, nullptr) == false){
        throw std::runtime_error("Не удалось получить доступ к принтеру");
    }

    if(handle != nullptr){
        _handle = *handle;
    }
}

string Printer::GetName() const{
    return _name;
}

HANDLE Printer::GetHandle() const{
    return _handle;
}
