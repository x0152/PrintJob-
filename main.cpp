#include "core.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");
    unique_ptr<Core> _core = std::make_unique<Core>();

    cout << "������ ���������� �� ������..." << endl;

    _core->SetCallbackAddJob([](Job job){
        cout << "�� ������ �������� ��������: " << job.GetNameJob() << ", ���:"
             << job.GetTypeDoc() <<  endl;
    });

    _core->SetCallbackUpdateJob([](Job job){
        cout << "��������� ������ ���������: " << job.GetNameJob() << ", ���:"
             << job.GetTypeDoc() << ", ���������� �������: " << job.GetTotalPage() << endl;
    });

    _core->SetCallbackDeleteJob([](Job job){
        cout << "������ ������: " << job.GetNameJob() << ", ���:"
             << job.GetTypeDoc() << ", ���������� �������: " << job.GetTotalPage()
             << ", ���������� ������������� �������: " << job.GetPrintedPage() << endl;
    });

    _core->SetCallbackPrintedJob([](Job job){
        cout << "����������� ������: " << job.GetNameJob() << ", ���:"
             << job.GetTypeDoc() << ", ���������� �������: " << job.GetTotalPage()
             << ", ���������� ������������� �������: " << job.GetTotalPage() << endl;
    });

    while(true){
        _core->Exec();
    }

    return 0;
}

