#include "core.h"

void Job::UpdateJob(function<void(Job)> cb_update){

    DWORD dwNeeded;
    JOB_INFO_2* pJobInfo;

    GetJob(_printer->GetHandle(), _id, 2, nullptr, 0, &dwNeeded);

    pJobInfo = new JOB_INFO_2[dwNeeded];

    if(dwNeeded != 0 && GetJob(_printer->GetHandle(), _id, 2, (LPBYTE)pJobInfo, dwNeeded, &dwNeeded)){
        _status = pJobInfo->Status;
        if(_position != pJobInfo->Position ||
            _totalPages != pJobInfo->TotalPages ||
            _pagesPrinted != pJobInfo->PagesPrinted) {

            _position = pJobInfo->Position;
            _totalPages = pJobInfo->TotalPages;
            _pagesPrinted = pJobInfo->PagesPrinted;

            if(cb_update){
                cb_update(*this);
            }
        }
    }

    delete pJobInfo;

}

void Jobs::HandleCompletedJobs(const std::function<void(Job)> cb_printed,
                            const std::function<void(Job)> cb_delete){

    for (auto& is : _exists ) {
        if (is.second == false){
            Job curJob = _jobs.find(is.first)->second;
            int status = curJob.GetStatus();
            if((status & JOB_STATUS_PRINTED) == JOB_STATUS_PRINTED){
                if(cb_printed){
                    curJob.UpdateJob([](Job){});
                    cb_printed(curJob);
                }
            }else{
                if(cb_delete){
                    curJob.UpdateJob([](Job){});
                    cb_delete(curJob);
                }
            }

            auto deleteJob = _jobs.find(is.first);
            if(deleteJob != _jobs.end()){
               _jobs.erase(deleteJob);
            }

            auto deleteIs = _exists .find(is.first);
            if(deleteIs != _exists .end()){
               _exists .erase(deleteIs);
            }
        }else{
            is.second = false;
        }

    }
}

void Jobs::JobAdd(JOB_INFO_2A* pJob, shared_ptr<Printer> printer, function<void(Job)> cb_addJob){

    if (pJob== nullptr){
       cout << "core.cpp:45" << endl;
    }

    if (_jobs.find(pJob->JobId) == _jobs.end()) {
        Job job((int)pJob->JobId,
                (char*)pJob->pDocument,
                (char*)pJob->pUserName,
                (char*)pJob->pMachineName,
                (char*)pJob->pDatatype,
                (int)pJob->Position,
                (int)pJob->TotalPages,
                (int)pJob->Status,
                (int)pJob->PagesPrinted,
                printer);

        _jobs.insert(std::make_pair(pJob->JobId, job));
        _exists .insert(std::make_pair(pJob->JobId, true));
        if(cb_addJob){
            cb_addJob(job);
        }
    }else{
        _exists [pJob->JobId] = true;
    }
}

Core::Core() {
    Init();
}

void Core::Init(){
    const int MAX_COUNT_PRINTER = 255;

    PRINTER_INFO_1A* printerInfo = new PRINTER_INFO_1A[MAX_COUNT_PRINTER];
    DWORD* countCopyBytes = new DWORD();
    DWORD* countStruct = new DWORD();

    if(EnumPrintersA(
       PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
       nullptr,
       1,
       (LPBYTE)printerInfo,
       sizeof(printerInfo) * MAX_COUNT_PRINTER,
       countCopyBytes,
       countStruct
    ) == 0){
        throw std::runtime_error("Не удалось получить список принтеров");
    }


    int countPrinters = *countStruct;
    for(int i = 0; i < countPrinters; ++i){
        string namePrinter(printerInfo[i].pName);
        shared_ptr<Printer> printer = std::make_shared<Printer>(namePrinter);
        _printers.push_back(printer);
    }

    delete printerInfo;
    delete countCopyBytes;
    delete countStruct;
}


bool Core::Exec(){

    for(auto& printer : _printers){
        DWORD dwNeeded;
        DWORD dwReturned;
        JOB_INFO_2A* pJobInfo;
        EnumJobs(printer->GetHandle(), 0, 0xFFFFFFFF, 2, NULL, 0, &dwNeeded,&dwReturned );
        pJobInfo = new JOB_INFO_2A[dwNeeded];

        if(dwNeeded != 0 && EnumJobsA(printer->GetHandle(), 0, 0xFFFFFFFF, 2, (LPBYTE)pJobInfo,dwNeeded, &dwNeeded, &dwReturned) ){
            _jobs.JobAdd(pJobInfo, printer, _cb_addJob);
        }

        delete pJobInfo;
    }

    _jobs.UpdateJobs(_cb_updateJob);
    _jobs.HandleCompletedJobs(_cb_printedJob, _cb_deleteJob);
}

Core::~Core(){

}

/*

        WORD wJobFields[7];
        wJobFields[0]           = JOB_NOTIFY_FIELD_PAGES_PRINTED;
        wJobFields[1]           = JOB_NOTIFY_FIELD_MACHINE_NAME;
        wJobFields[2]           = JOB_NOTIFY_FIELD_STATUS;
        wJobFields[3]           = JOB_NOTIFY_FIELD_TOTAL_PAGES;
        wJobFields[4]           = JOB_NOTIFY_FIELD_DOCUMENT;
        wJobFields[5]           = JOB_NOTIFY_FIELD_STATUS_STRING;
        wJobFields[6]           = JOB_NOTIFY_FIELD_START_TIME;

        PRINTER_NOTIFY_OPTIONS_TYPE rOptions[1];
        rOptions[0].Type    = JOB_NOTIFY_TYPE;
        rOptions[0].pFields = &wJobFields[0];
        rOptions[0].Count   = 7;

        PRINTER_NOTIFY_OPTIONS* option = new PRINTER_NOTIFY_OPTIONS;
        option->Count = 1;
        option->Version = 2;
        option->pTypes = rOptions;
        option->Flags = PRINTER_NOTIFY_OPTIONS_REFRESH;

        HANDLE handleJobs = FindFirstPrinterChangeNotification(printer->GetHandle(), PRINTER_CHANGE_ALL, 0, option);

        EnumJobs(printer->GetHandle(), 0, 0xFFFFFFFF, 2, nullptr, 0, &dwNeeded, &dwReturned);
        int sizeBuffer = dwNeeded;
        char* jobs = new char(sizeBuffer);

        //EnumJobs(printer->GetHandle(), 0, 0xFFFFFFFF, 2, (LPBYTE)jobs, sizeBuffer, &dwNeeded, &dwReturned);

        //JOB_INFO_2* job = (JOB_INFO_2*)jobs;


        if(handleJobs != INVALID_HANDLE_VALUE){

            cout << "starting..."<< endl;
            if (WaitForSingleObject(handleJobs, 10) == WAIT_OBJECT_0){
                cout << "starting2..."<< endl;
                WORD* msg = new WORD();
                PRINTER_NOTIFY_INFO* infoPrint;

                #define CHECK_TASK(NAME) \
                if((*msg & NAME) == NAME){\
                    cout << #NAME << endl;\
                }


                if(FindNextPrinterChangeNotification(handleJobs, (PDWORD)msg, (LPVOID)option, (LPVOID*)&infoPrint) == true){

                    if((*msg & PRINTER_CHANGE_ADD_JOB) == PRINTER_CHANGE_ADD_JOB){
                        EnumJobs(printer->GetHandle(), 0, 0xFFFFFFFF, 2, nullptr, 0, &dwNeeded, &dwReturned);
                        int sizeBuffer = dwNeeded;
                        char* jobs = new char(sizeBuffer);

                        EnumJobs(printer->GetHandle(), 0, 0xFFFFFFFF, 2, (LPBYTE)jobs, sizeBuffer, &dwNeeded, &dwReturned);

                        JOB_INFO_2* job = (JOB_INFO_2*)jobs[0];

                    }

                    CHECK_TASK(PRINTER_CHANGE_FAILED_CONNECTION_PRINTER)
                    CHECK_TASK(PRINTER_CHANGE_PRINTER)
                    CHECK_TASK(PRINTER_CHANGE_DELETE_JOB)
                    CHECK_TASK(PRINTER_CHANGE_ADD_JOB)
                    CHECK_TASK(PRINTER_CHANGE_JOB )
                    CHECK_TASK(PRINTER_CHANGE_ADD_FORM )
                    CHECK_TASK(PRINTER_CHANGE_SET_FORM )
                    CHECK_TASK(PRINTER_CHANGE_DELETE_FORM )
                    CHECK_TASK(PRINTER_CHANGE_FORM )
                    CHECK_TASK(PRINTER_CHANGE_ADD_PORT )
                    CHECK_TASK(PRINTER_CHANGE_CONFIGURE_PORT )
                    CHECK_TASK(PRINTER_CHANGE_DELETE_PORT )
                    CHECK_TASK(PRINTER_CHANGE_PORT )
                    CHECK_TASK(PRINTER_CHANGE_ADD_PRINT_PROCESSOR )
                    CHECK_TASK(PRINTER_CHANGE_DELETE_PRINT_PROCESSOR )
                    CHECK_TASK(PRINTER_CHANGE_PRINT_PROCESSOR )
                    CHECK_TASK(PRINTER_CHANGE_ADD_PRINTER_DRIVER )
                    CHECK_TASK(PRINTER_CHANGE_SET_PRINTER_DRIVER )
                    CHECK_TASK(PRINTER_CHANGE_DELETE_PRINTER_DRIVER )
                    CHECK_TASK(PRINTER_CHANGE_PRINTER_DRIVER )
                    CHECK_TASK(PRINTER_CHANGE_TIMEOUT)
                    CHECK_TASK(PRINTER_CHANGE_SET_JOB)
                    CHECK_TASK(PRINTER_CHANGE_WRITE_JOB )
                    CHECK_TASK(PRINTER_CHANGE_TIMEOUT)

                }

                FindClosePrinterChangeNotification(handleJobs);
                delete msg;
            }
        }
        delete option;*/
