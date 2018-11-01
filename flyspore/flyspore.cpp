// flyspore.cpp: 定义控制台应用程序的入口点。
//

#include "core/task_pool.h"
#include "Threads.h"

#include "core/Spore.h"
#include "core/Context.h"
#include "core/Session.h"
#include "core/DataPack.h"
#include "core/Path.h"
#include "core/Schema.h"
using namespace fs;

void testDefaultSchema()
{
	auto _startTime = std::chrono::high_resolution_clock::now();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = 0.0f;

	auto mainSpore = Spore::newSpore("Main Space");

	auto entryPin = defaultSchema()->addPin(mainSpore, "entry", Pin_Type::IN_PIN);


	auto L1 = mainSpore->addChild("sporeL1");
	auto fun_L1 = [&](Context& cc, DataPack& data) {
		static std::mutex _lock;

		auto &nLevel = data.ref<int>("Level");
		auto &maxLevel = data.ref<int>("MaxLevel");
		auto &expCount = data.ref<int>("expCount");

		auto &currentLevel = cc.session.ref<int>("currentLevel");
		{
			cc.session.ref< std::atomic_ullong>("Count").fetch_add(1);
		}

		if (currentLevel < nLevel)
		{
			currentLevel = nLevel;
			std::lock_guard<std::mutex> lock(_lock);
			std::cout << "! new Level: " << nLevel;
			auto _currentTime = std::chrono::high_resolution_clock::now();
			auto elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
			std::cout << "  elapsed time: " << elapsed_seconds << "s\n";
		}
		if (nLevel >= maxLevel)
			return;

		for (int n = 0; n < expCount; n++)
		{
			auto outData = std::make_shared<DataPack>();
			outData->ref<int>("Level") = nLevel + 1;
			outData->ref<int>("MaxLevel") = maxLevel;
			outData->ref<int>("expCount") = expCount;
			cc.push("out", outData);
		}
	};
	auto out_L1 = defaultSchema()->addPin(L1, "out");
	auto in_L1 = defaultSchema()->addPin(L1, "in", fun_L1);

	Path::connect(entryPin, in_L1);
	Path::connect(out_L1, in_L1);


	auto session = Session::newSession(entryPin);
	session->values().ref< std::atomic_ullong>("Count");
	AnyValues originData;
	originData.ref<int>("Level") = 0;
	originData.ref<int>("MaxLevel") = 6;
	originData.ref<int>("expCount") = 10;

	session->submit(originData);
	session->join();

	auto finishCount = session->values().ref<std::atomic_ullong>("Count").load();

	std::cout << "Session finish, the result:" << finishCount << "\n";

	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	task_pool &_tp = task_pool::get_instance();
	std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";
}

void testMySchema()
{
	auto _startTime = std::chrono::high_resolution_clock::now();
	auto _currentTime = std::chrono::high_resolution_clock::now();
	double elapsed_seconds = 0.0f;

	class MyDataPack : public Data
	{
	public:
		MyDataPack() :Data() {}
		int Level{0};
		int MaxLevel{ 0 };
		int expCount{ 0 };
	};

	using myProcess = void(Context&, MyDataPack&);
	auto schema = std::make_shared<Schema< myProcess>>(
		([](std::function<myProcess> innerProcess, Context& context, const P_Data& pdata) {
		//transform...
		MyDataPack* pvs = (MyDataPack*)pdata.get();
		if (innerProcess && pvs)
		{
			innerProcess(context, *pvs);
		}
	}),
	([](const AnyValues &values) -> P_Data {
		std::shared_ptr< MyDataPack> mypack = std::make_shared<MyDataPack>();
		values.get<int>("Level", mypack->Level);
		values.get<int>("MaxLevel", mypack->MaxLevel);
		values.get<int>("expCount", mypack->expCount);
		return std::move(mypack);
	}),
	([](const P_Data& pdata) ->AnyValues
	{
		AnyValues anyValues;
		MyDataPack* p = (MyDataPack*)pdata.get();
		if (p)
		{
			anyValues.ref<int>("Level") = p->Level;
			anyValues.ref<int>("MaxLevel") = p->MaxLevel;
			anyValues.ref<int>("expCount") = p->expCount;
		}
		return std::move(anyValues);
	}));


	auto mainSpore = Spore::newSpore("Main Space");
	auto entryPin = schema->addPin(mainSpore, "entry", Pin_Type::IN_PIN);

	auto L1 = mainSpore->addChild("sporeL1");
	auto fun_L1 = [&](Context& cc, MyDataPack& data) {
		static std::mutex _lock;

		auto &nLevel = data.Level;
		auto &maxLevel = data.MaxLevel;
		auto &expCount = data.expCount;

		auto &currentLevel = cc.session.ref<int>("currentLevel");
		cc.session.ref< std::atomic_ullong>("Count").fetch_add(1);
		if (currentLevel < nLevel)
		{
			currentLevel = nLevel;
			std::lock_guard<std::mutex> lock(_lock);
			std::cout << "! new Level: " << nLevel;
			auto _currentTime = std::chrono::high_resolution_clock::now();
			auto elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
			std::cout << "  elapsed time: " << elapsed_seconds << "s\n";
		}
		if (nLevel >= maxLevel)
			return;

		for (int n = 0; n < expCount; n++)
		{
			auto outData = std::make_shared<MyDataPack>();
			outData->Level = nLevel + 1;
			outData->MaxLevel = maxLevel;
			outData->expCount = expCount;
			cc.push("out", outData);
		}
	};
	auto out_L1 = schema->addPin(L1, "out");
	auto in_L1 = schema->addPin(L1, "in", fun_L1);

	Path::connect(entryPin, in_L1);
	Path::connect(out_L1, in_L1);


	auto session = Session::newSession(entryPin);
	session->values().ref< std::atomic_ullong>("Count");
	AnyValues originData;
	originData.ref<int>("Level") = 0;
	originData.ref<int>("MaxLevel") = 6;
	originData.ref<int>("expCount") = 10;

	session->submit(originData);
	session->join();

	auto finishCount = session->values().ref<std::atomic_ullong>("Count").load();

	std::cout << "Session finish, the result:" << finishCount << "\n";

	_currentTime = std::chrono::high_resolution_clock::now();
	elapsed_seconds = ((std::chrono::duration<double>)(_currentTime - _startTime)).count();
	std::cout << "  elapsed time: " << elapsed_seconds << "s | avg: " << finishCount / elapsed_seconds << "\n";
	task_pool &_tp = task_pool::get_instance();
	std::cout << "All task finish,ths  count is :" << _tp.get_finish_count() << "\n";
}

int main()
{
	testDefaultSchema();
	testMySchema();
	char _c;
	std::cin >> _c;
	return 0;

}

