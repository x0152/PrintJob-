#ifndef CORE_H
#define CORE_H
#include <string>
#include <iostream>
#include <functional>
#include <map>
#include "printer.h"
//#include "includes.h"

using std::function;
using std::map;
using std::string;
using std::cout;
using std::endl;
using std::begin;
using std::end;


typedef list<shared_ptr<Printer>> Printers;

class Job{
public:
    Job(){}
    Job(const int id,
        const string nameJob,
        const string userName,
        const string machineName,
        const string typeData,
        const int position,
        const int totalPages,
        const int status,
        const int pagesPrinted,
        shared_ptr<Printer> printer)
        {

        _id = id;
        _nameJob = nameJob;
        _userName = userName;
        _machineName = machineName;
        _typeData = typeData;
        _position = position;
        _totalPages = totalPages;
        _status = status;
        _pagesPrinted = pagesPrinted;
        _printer = printer;
    }

    void UpdateJob(function<void(Job)> cb_update);

    string GetNameJob() const {return _nameJob;}
    int GetPrintedPage() const {return _pagesPrinted;}
    int GetTotalPage() const {return _totalPages;}
    int GetStatus() const {return _status;}
    string GetTypeDoc() const {return _typeData;}
    string GetUser() const {return _userName;}
    string GetPCName() const {return _machineName;}
private:
    int _id;
    string _nameJob;

    string _userName;
    string _machineName;
    string _typeData;
    int _position;
    int _totalPages;
    int _status;
    int _pagesPrinted;
    shared_ptr<Printer> _printer;
};

class Jobs{
public:
    void UpdateJobs(function<void(Job)> cb_update){
        for (auto& job : _jobs) {
           job.second.UpdateJob(cb_update);
        }
    }

    void HandleCompletedJobs(const std::function<void(Job)> cb_print,
                          const std::function<void(Job)> cb_delete);

    void JobAdd(JOB_INFO_2A* pJob,
                shared_ptr<Printer> printer,
                function<void(Job)> cb_addJob);
private:
    map<int, Job> _jobs;
    map<int, bool> _exists;
};

class Core {
public:
    Core();
    ~Core();
    void Init();
    bool Exec();
    void Clear();
    void SetCallbackAddJob(const std::function<void(Job)> cb){_cb_addJob = cb;}
    void SetCallbackUpdateJob(const std::function<void(Job)> cb){_cb_updateJob = cb;}
    void SetCallbackDeleteJob(const std::function<void(Job)> cb){_cb_deleteJob = cb;}
    void SetCallbackPrintedJob(const std::function<void(Job)> cb){_cb_printedJob = cb;}
private:
    function<void(Job)> _cb_addJob;
    function<void(Job)> _cb_updateJob;
    function<void(Job)> _cb_deleteJob;
    function<void(Job)> _cb_printedJob;
    Printers _printers;
    Jobs _jobs;
};

#endif // CORE_H
