#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

class ThreadTest : public ::testing::Test {
protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {}
};

/*thread_local*/
unsigned int rage = 1;
std::mutex cout_mutex;

void increase_rage(const std::string& thread_name)
{
	++rage;
	std::lock_guard<std::mutex> lock(cout_mutex);
	std::cout << "Rage counter for " << thread_name << ": " << rage << '\n';
}

TEST_F(ThreadTest, IsEmptyInitially) {
	std::thread a(increase_rage, "a"), b(increase_rage, "b");

	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "Rage counter for main: " << rage << '\n';
	}

	a.join();
	b.join();

	EXPECT_EQ(rage, 3);
}

#include <iostream>
#include <future>
#include <thread>

TEST_F(ThreadTest, packaged_task){
	// future from a packaged_task
	std::packaged_task<int()> task([](){ return 7; }); // wrap the function
	std::future<int> f1 = task.get_future();  // get a future
	std::thread(std::move(task)).detach(); // launch on a thread

	

	std::cout << "Waiting..." << std::flush;
	f1.wait();
	int ret = f1.get();
	std::cout << "Done!\nResults are: " << ret << '\n';

	EXPECT_EQ(ret, 7);
	
	/*
	shared_future

	(C++11)

	waits for a value (possibly referenced by other futures) that is set asynchronously
	*/
};

TEST_F(ThreadTest, asyncTask){
	

	// future from an async()
	/*
		async::runs a function asynchronously (potentially in a new thread) and returns a std::future that will hold the result
	*/
	std::future<int> f2 = std::async(std::launch::async, [](){ return 8; });

	
	std::cout << "Waiting..." << std::flush;
	f2.wait();
	int ret = f2.get();
	std::cout << "Done!\nResults are: " << ret  << '\n';

	EXPECT_EQ(ret, 8);
	

	/*
	shared_future

	(C++11)

	waits for a value (possibly referenced by other futures) that is set asynchronously
	*/
};





TEST_F(ThreadTest, promise){
	// future from a promise
	
	auto p = std::promise<int>();
	auto f3 = p.get_future();

	/*
		 	std::thread([](std::promise<int> p){p.set_value_at_thread_exit(9); }, std::move(p)).detach();
			wrong=>原文参数为 std::promise<int> p，编译失败，必须是std::promise<int>& p
			wrong=>set_value_at_thread_exit 一运行程序就退出，很奇怪
	*/
	std::thread([](std::promise<int>& p){p.set_value(9); }, std::move(p)).detach();
	
	std::cout << "Waiting..." << std::endl;
	//f3.wait();//wait不调用似乎也可以
	int ret = f3.get();
	std::cout << "Done!\nResults are: " 	<< ret << '\n';

	EXPECT_EQ(ret, 9);

	
};




template<typename F>
auto async1(F&& func) -> std::future<decltype(func())>
{
	typedef decltype(func()) result_type;

	auto promise = std::promise<result_type>();
	auto future = promise.get_future();

	std::thread(std::bind([=](std::promise<result_type>& promise)
	{
		try
		{
			promise.set_value(func()); // Note: Will not work with std::promise<void>. Needs some meta-template programming which is out of scope for this question.
		}
		catch (...)
		{
			promise.set_exception(std::current_exception());
		}
	}, std::move(promise))).detach();

	return std::move(future);
}

template<typename F>
auto async2(F&& func) -> std::future<decltype(func())>
{
	auto task = std::packaged_task<decltype(func())()>(std::forward<F>(func));
	auto future = task.get_future();

	std::thread(std::move(task)).detach();

	return std::move(future);
}



template<class _Fty,
class... _ArgTypes> 
	auto async3(_Fty&& _Fnarg, _ArgTypes&&... _Args)
			-> std::future<typename std::result_of<_Fty(_ArgTypes...)>::type>
{	// return a future object whose associated asynchronous state
	// manages a function object
	typedef typename std::result_of<_Fty(_ArgTypes...)>::type _Ret;
	auto task = std::packaged_task<_Ret()>(std::bind(std::forward<_Fty>(_Fnarg), std::forward<_ArgTypes>(_Args)...));
	//auto task = std::packaged_task<_Ret()>(_Fnarg(std::forward<_ArgTypes>(_Args)...)); this is ok too.
	auto future = task.get_future();
	std::thread(std::move(task)).detach();

	return std::move(future);
	
}





TEST_F(ThreadTest, async3){
	auto ret = async3([](){return 10; });
	int i = ret.get();
	EXPECT_EQ(i, 10);

	ret = async3([](int x){return 10+x; },10);
	EXPECT_EQ(ret.get(), 20);

	struct task
	{
		int operator()(int x){
			return x + 5;
		}
	};

	ret = async3(task(), 10);
	EXPECT_EQ(ret.get(), 15);
	
}

/////////////////////////////////////////
#include <vector>
#include <thread>
#include <utility>

void Simple2(int a, int b) {
	std::cout << "jack" << std::endl;
}

template <typename Function, typename... Args>
void Wrapper(Function&& f, Args&&... a) {
	f(std::forward<Args>(a)...);
}

class Pool {
public:
	//template<class _Fn,
	//class... _Args>
	//	explicit thread(_Fn&& _Fx, _Args&&... _Ax)
	//{	// construct with _Fx(_Ax...)
	//	_Launch(&_Thr,
	//		_STD bind(_Decay_copy(_STD forward<_Fn>(_Fx)),
	//		_Decay_copy(_STD forward<_Args>(_Ax))...));
	//}

	//template<class _Target> inline
	//	void _Launch(_Thrd_t *_Thr, _Target&& _Tg)
	//{	// launch a new thread
	//	_LaunchPad<_Target> _Launcher(_STD forward<_Target>(_Tg));
	//	_Launcher._Launch(_Thr);
	//}

	//template<class _Other> inline
	//	_LaunchPad(_Other&& _Tgt)
	//	: _MyTarget(_STD forward<_Other>(_Tgt))
	//{	// construct from target
	//}

	template <typename Function, typename... Args>
	void Binder(Function&& f, Args&&... a) {
		std::thread t(Wrapper<typename std::decay<Function>::type, Args...>,
			std::forward<Function>(f), std::forward<Args>(a)...);
	}
};

class Pool1 {
public:
	template <typename Function, typename... Args>
	void Binder(Function&& f, Args&&... a) {
		std::thread t(Wrapper<Function, Args...>,
			std::forward<Function>(f), std::forward<Args>(a)...);
	}
};

class Pool3 {
public:
	template <typename Function, typename... Args>
	void Binder(Function&& f, Args&&... a) {
		std::thread t(Wrapper<std::decay_t<Function>, Args...>,
			std::forward<Function>(f), std::forward<Args>(a)...);
	}
};



template<class _Target> inline
void Wrapper4(_Target&& _Tg)
{	// launch a new thread
	(_Tg)();
}

class Pool4{
public:
	Pool4(){}

	template<class _Fn,
	class... _Args>
		explicit Pool4(_Fn&& _Fx, _Args&&... _Ax)
	{	// construct with _Fx(_Ax...)
		Binder(std::forward<_Fn>(_Fx),	std::forward<_Args>(_Ax)...);
	}

	template<class _Fn,
	class... _Args>
		void Binder(_Fn&& _Fx, _Args&&... _Ax)
	{	// construct with _Fx(_Ax...)
		Wrapper4(std::bind(std::_Decay_copy(std::forward<_Fn>(_Fx)),
				std::_Decay_copy(std::forward<_Args>(_Ax))...));

		
	}

	template<class _Fn,
	class... _Args>
		void Binder1(_Fn&& _Fx, _Args&&... _Ax)
	{	// construct with _Fx(_Ax...)
		mFn = std::bind(std::_Decay_copy(std::forward<_Fn>(_Fx)),
			std::_Decay_copy(std::forward<_Args>(_Ax))...);

	}
	
	std::function<void(int)>  mFn;
};

TEST_F(ThreadTest, Variadic_Args){
	Pool4().Binder(Simple2, 3, 4);
	Pool4(Simple2, 3, 4);

	Pool4 j;
	//j.Binder1(Simple2, 2);
	//j.mFn(3);


	auto t = std::bind(Simple2, 2,std::placeholders::_1);
	t(3);

	auto t1 = std::bind(Simple2, 2,3);
	t1(3);

//	auto t3 = std::bind(Simple2, 2);
//	t3(3);

}

