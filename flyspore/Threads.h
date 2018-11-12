/*!
 * \file Threads.h
 * \date 2016/12/06 18:05
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: thread demo
 *
 * \note
*/

// *** ADDED BY HEADER FIXUP ***
#include <cassert>
#include <istream>
// *** END ***
#ifndef THREADS_H_INCLUDED
#define THREADS_H_INCLUDED

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <memory>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <future>
#include <type_traits>
#include <sstream>

#include "core/Level0/stealing_queue.h"
#include "core/Level0/task_pool.h"
using namespace std;

#ifdef _MSC_VER

#else

#define _MAX_INT_DIG 32
#define _TOSTRING(buf, bufsize,fmt, val)	\
	snprintf(buf,  bufsize, fmt, val)

inline string to_string(int _Val)
{	// convert long to string
	char _Buf[2 * _MAX_INT_DIG];
	_TOSTRING(_Buf, sizeof(_Buf), "%d", _Val);
	return (string(_Buf));
}

inline string to_string(long _Val)
{	// convert long to string
	char _Buf[2 * _MAX_INT_DIG];

	_TOSTRING(_Buf, sizeof(_Buf), "%ld", _Val);
	return (string(_Buf));
}

inline string to_string(unsigned long _Val)
{	// convert unsigned long to string
	char _Buf[2 * _MAX_INT_DIG];

	_TOSTRING(_Buf, sizeof(_Buf), "%lu", _Val);
	return (string(_Buf));
}

#endif

void g_do_some_work(int v1, int v2, string &result) {
	this_thread::sleep_for(chrono::milliseconds(100));

	result = R"(Static combind Result:)";
}

void g_do_some_work2(int nv, string &result) {
	std::cout << R"(g_do_some_work2:)" << to_string(nv) << " | " << result;
}

string g_do_some_work3(int nv, string &result) {
	std::cout << R"(g_do_some_work3:)" << to_string(nv) << " | " << result;
	return result;
}

string g_do_some_work6(int nv, int nv2, string &result) {
	std::ostringstream  ss;
	ss << R"(g_do_some_work6:)" << nv << " | " << nv2 << result;
	return ss.str();
}

void g_process_unique_obj(unique_ptr<int> val) {
	this_thread::sleep_for(chrono::milliseconds(100));
}

///////////////////////////////////////////////////

/////////////////////////////////////////////////////

int test_Threads(){

   	/*
	thread������

	thread�Ķ�����캯���ṩ�˲�ͬ��������ʽ�������ǹ���ʱʹ�ñ䳤����ģ���move�������ṩ�ǳ��������������󶨡�
	�����������޶���������Ϊ��̬��������C++11֮ǰ��ƽ̨��ص�������������Windows�µ�::CreateThread�������ڲ�ʹ��������bind�������ϳ�������ڡ�
	*/

	//eg.1.��ͳ���߳�������ʽ����̬�������
	string strresult;
	//��Ҫע����ǣ��̺߳������ݲ���ʱ�ǰ�ֵ���ݣ������Ҫ�������ã�ʹ��std::ref��װ�������װ�ڲ�ʹ����reference_wrapper��
	thread th1(g_do_some_work, 11, 22, std::ref(strresult));
	th1.join();



	/*
	������Խ�����ͬ�����ĺ���ͨ��bind��һ��Ϊһ������ͳһ�ĺ���ָ���װ��std::function<void()>����Ȼ��ŵ��������������ͨ�������̳߳�������ִ������
	*/
	cout << "\n-------------task_list-------------------\n";
	std::packaged_task<int()> task([](){return 7;}); // wrap the function
	std::cout <<  "sizeof task packaged is:" << sizeof(task) << std::endl;

    std::tuple<int, int, int, int, int, int, std::string> t1(1, 2, 3, 4, 5, 6, std::string("aaaa"));
    int nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string>);
     nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string, int>);
     nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string, std::vector<int>>);
     nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string, std::string>);
    std::cout <<  "tuple size  is:" << nsize << std::endl;

    std::future<int> result = task.get_future();  // get a future
    auto pack_task = std::bind(std::move(task));

	typedef std::function<void()> task_type;
	strresult = "task__running __in__list\n";
    auto work_call2  = std::bind(g_do_some_work2, 2, std::ref(strresult));
    auto work_call3  = std::bind(g_do_some_work2, 3, std::ref(strresult));
    auto work_call4  = std::bind(g_do_some_work2, 4, std::ref(strresult));

    strresult = "\n------->>task__running __in__work6------------\n";
    auto work_call6  = std::bind(g_do_some_work6, 4, 5, std::ref(strresult));

    std::packaged_task<std::string()> task2(std::move(work_call6)); // wrap the function
	std::cout <<  "sizeof task2 packaged is:" << sizeof(task2) << std::endl;
    auto _task2_future = task2.get_future();
	{
		//std::unique_ptr<kcc::taskWrap> _tw = std::make_unique<kcc::anyTaskWrap<std::string>>(std::move(task2));
		//(*_tw)();
		//string _task2_result = _task2_future.get();
		//std::cout << _task2_result;
	}


    //std::queue<std::packaged_task<void()>> taskQueue;
    //taskQueue.push(std::move(task2));

    auto work_call5  = std::bind(g_do_some_work3, 100, std::ref(strresult));

    std::vector<task_type> tasks;
    tasks.push_back(std::move(work_call2));
    tasks.push_back(std::move(work_call3));
    tasks.push_back(std::move(work_call4));
    tasks.push_back(std::move(work_call5));
   //tasks.push_back(std::move(work_call6));

    //tasks.push_back(std::move(pack_task));

    for(auto &task : tasks){
        task();
    }

    task_type taskA;
    if(tasks.size() > 0)
    {
        taskA = std::move(tasks.back());
        tasks.pop_back();
    }
    taskA();
    cout << "\n------------------------------------------\n";



    //std::function<void()> fc = std::move(work_call2);


	//eg.2. �����ṩ��operator()����������ʵ��
	class do_some_work_man {
		string base;
	public:
		do_some_work_man(const string &b) :base(b) {}
		void operator() (int v1, int v2, string &result) {
			result = base;
		}
	};
	thread th2(do_some_work_man(string("th2 Combin Result:")), 22, 33, std::ref(strresult));
	th2.join();




	//eg.3. ����lambda
	thread th3([base = string("th3 Combin Result")](int v1, int v2, string &result){
		result = base + to_string(v1) + "__" + to_string(v2);
	}, 33, 44, std::ref(strresult));
	th3.join();
	//���⣬����lambda�ķ������������Ըó��������ʽ��
	thread th4([base = string("th4 Combin Result"), &strresult](int v1, int v2){
		strresult = base + to_string(v1) + "__" + to_string(v2);
	}, 33, 44);
	th4.join();



	//eg.4. ���ݳ�Ա����
	class do_some_work_man2 {
		string base;
	public:
		do_some_work_man2(const string &b) :base(b) {}
		void run(int v1, int v2, string &result) {
			result = base + to_string(v1) + "__" + to_string(v2);
		}
	};
	do_some_work_man2 man2(string("th5 Combin Result:"));
	thread th5(&do_some_work_man2::run, &man2, 22, 33, std::ref(strresult));
	th5.join();



	//���⣬����Ҳ����ʹ��bind��������������
	do_some_work_man2 man3(string("th6 Combin Result:"));
	auto work_call = std::bind(&do_some_work_man2::run, &man3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	thread th6(work_call, 55, 66, std::ref(strresult));
	th6.join();



	/*
		�ȴ��߳�ִ�����
	*/

	/*
		thread�ڹ���ʱ������ʼ�����̣߳���ʼ������ֱ���߳̽�������ʹthread��������������Ѿ�������
		thread::join()��������ǰ�̣߳�ֱ���������߳����Ϊֹ���������ϣ��thread���������ں��߳�ͬʱ����ʱ������ʹ��RAII����
	*/

	//eg.5.�ȴ��߳����
	class thread_guard{
		std::thread &t;
	public:
		explicit thread_guard(std::thread &t_):t(t_){}
		~thread_guard() { if (t.joinable()) t.join(); }
		thread_guard(thread_guard const&) = delete;				//�رտ������캯��
		thread_guard& operator=(thread_guard const&) = delete;  //�رո�ֵ����
	};

	thread th7([]() {
		for (auto i = 0; i < 30; i ++){
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	});
	{
		thread_guard tg(th7);
	}
	assert(!th7.joinable());


	//eg.6. ��̨�����߳�

	thread th8(g_do_some_work, 11, 22, ref(strresult));
	/*
		detch()���������̵߳Ĺ����� ����ϵͳ�������ͷ���ֹ�̵߳���Դ,��Ч�����̶߳���������ͬ��
		��ȷ�ĵ�������Ŀ�����ڱ�������ȷʵ��Ҫ��������߳���Դ������������join��������
	*/
	th8.detach();
	assert(!th8.joinable());


	/*
		�������̴߳��ݲ�����Ҫע��ĵط�

		ǰ��˵���߳�����ʱ�ǰ�ֵ���ݲ�������Ҫʱʹ��std::ref���÷�װ��������һЩ���ܿ����Ĳ������ԣ���Ҫʹ��move������
	*/

	//eg.7.

	auto process_unique_obj  = [](unique_ptr<int> val) {
		this_thread::sleep_for(chrono::milliseconds(1));
		std::cout << *val;// << endl;
	};
	unique_ptr<int> u_val = make_unique<int>(100);

	//�����ʹ��std::move��������ֱ������ C2280������unique_ptr�Ŀ������캯����ɾ��
	thread th9(process_unique_obj, std::move(u_val));
	th9.join();

	//thread th10(process_unique_obj, std::ref(u_val));
	//th10.join();

	return 0;
}


#endif // THREADS_H_INCLUDED
