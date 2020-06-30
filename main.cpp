#include "core.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");
    unique_ptr<Core> _core = std::make_unique<Core>();

    cout << "Ожидаю документов на печать..." << endl;

    _core->SetCallbackAddJob([](Job job){
        cout << "На печать добавлен документ: " << job.GetNameJob() << ", Тип: "
             << job.GetTypeDoc() <<  endl;
    });

    _core->SetCallbackUpdateJob([](Job job){
        cout << "Изменения данных документа: " << job.GetNameJob() << ", Тип: "
             << job.GetTypeDoc() << ", количество страниц: " << job.GetTotalPage() << endl;
    });

    _core->SetCallbackDeleteJob([](Job job){
        cout << "Отмена печати: " << job.GetNameJob() << ", Тип: "
             << job.GetTypeDoc() << ", количество страниц: " << job.GetTotalPage()
             << ", количество распечатанных страниц: " << job.GetPrintedPage() << endl;
    });

    _core->SetCallbackPrintedJob([](Job job){
        cout << "Завершение печати: " << job.GetNameJob() << ", Тип: "
             << job.GetTypeDoc() << ", количество страниц: " << job.GetTotalPage()
             << ", количество распечатанных страниц: " << job.GetTotalPage() << endl;
    });

    while(true){
        _core->Exec();
    }

    return 0;
}

