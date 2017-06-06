// asio_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <boost/asio.hpp>

#include "asio_generic_server.hpp"
#include "test_handler.h"


int _tmain(int argc, _TCHAR* argv[])
{

    asio_gengric_server<test_handler> server;
    server.start_server(10086);

    {
        boost::asio::io_service ios;
        test_handler oo(ios);
        std::thread td([&](){ ios.run(); });
        ios.stop();
        td.join();
    }

    server.stop_server();
	return 0;
}

