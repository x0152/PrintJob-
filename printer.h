#ifndef PRINTER_H
#define PRINTER_H
#include "includes.h"

class Printer
{
public:
    Printer(const string name);
    string GetName() const;
    HANDLE GetHandle() const;
private:
    HANDLE _handle;
    string _name;
};

#endif // PRINTER_H
