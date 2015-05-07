#define TGMLOG_CHANNEL "loop_tesst"
#include "tgmlog/tgmlog.h"


#include <tangram/loop/Looper.h>
#include <tangram/loop/Handler.h>
#include <tangram/loop/HandlerThread.h>
#include <utils/String8.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <iterator>

using namespace tangram;
using namespace tangram::loop;



/*
* <pre>
class LooperThread extends THREAD {
public:
class _MyHandler:Handler{
public:
void handleMessage(Message msg) {
// process incoming messages here
}
};

SP<Handler> mHandler;
void run() {
Looper.prepare();
mHandler = new _MyHandler();
Looper.loop();
}
}
*/

static bool _handle_msg(SP<Message>& msg){
	std::cout << "3*** what=" << msg->what << std::endl;
	return false;
}

//使用HandlerThread来封装Looper::prepare,Looper::loop
class LooperThread :public HandlerThread{
public:
	virtual void onLooperPrepared(){
        //绑定处理函数到类成员方法
		mHandler2.reset(new Handler(std::bind(&LooperThread::handleMessage, this,std::placeholders::_1)));
		//mHandler2.reset(new Handler(&LooperThread::handleMessage, this));

        //绑定处理函数到全局函数
		mHandler3.reset(new Handler(_handle_msg));
		
	}
	
	bool handleMessage(SP<Message>& msg)
	{
		std::cout << "2*** what=" << msg->what << std::endl;
		return true;
	}

	SP<Handler> mHandler2;
	SP<Handler> mHandler3;
};



int main(){
    	
	//临时线程
	SP<Handler> h1;
	std::thread t1([](SP<Handler>& h1){
		Looper::prepare();
		//h1.reset((Handler*)(new MyHandler()));
		h1.reset(new Handler([](SP<Message>& msg){
			switch (msg->what)
			{
			case 1:
			{
				std::cout << "t1 get message:" << msg->data.GetString8(String16("msg")).string() << std::endl;
				return true;
			}
			break;
			default:
				WARN("unknown Message!\n");
				return false;
			}

		}));
		Looper::loop();
	}, std::ref(h1));


    SP<Handler> h2;
    std::thread t2([&h2](){
        Looper::prepare();
        //h1.reset((Handler*)(new MyHandler()));
        h2.reset(new Handler([](SP<Message>& msg){
            switch (msg->what)
            {
            case 1:
            {
                std::cout << "t2 get message:" << msg->data.GetString8(String16("msg")).string() << std::endl;
                return true;
            }
            break;
            default:
                WARN("unknown Message!\n");
                return false;
            }

        }));
        Looper::loop();
    });
	t2.detach();

	//HandlerThread

	//线程类
	LooperThread lt;
	lt.start();

	//ctrl+z to end the input
	/*
	*/
	std::cout << "Please input Message and to exit with CTRL+Z." << std::endl;
	for (auto s = std::istream_iterator<std::string> {std::cin}; 
		s != std::istream_iterator<std::string> {};	
		s++)
    {
		std::cout << ">>";
		SP<Message> m = Message::obtain();
		m->what = 1;
		m->data.PutString8(String16("msg"),String8(s->c_str()));
		h1->sendMessage(m);

        //一个消息不能发送到两个队列？
		m = Handler::getPostMessage([](){std::cout << "----"<< std::endl; });
		h1->sendMessage(m);

		lt.mHandler2->sendEmptyMessageDelayed(2);
		lt.mHandler3->sendEmptyMessageDelayed(3);
		
	}
	
	//std::cin.clear();
	//std::cout << "Press any key..." << std::endl;
	//char c;
	//std::cin.getline(&c, 1);
   	return 0;
}