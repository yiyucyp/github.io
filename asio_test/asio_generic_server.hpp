#pragma once
#include <thread>
#include <boost/asio.hpp>

template<typename connect_handler>
class asio_gengric_server
{
    using shared_hander_t = std::shared_ptr<connect_handler>;

public:
    asio_gengric_server(int thread_count = 1)
        : thread_count_(thread_count)
        , acceptor_(io_service_)
    {

    }

    void start_server(std::uint16_t port)
    {
        auto handler = std::make_shared<connect_handler>(io_service_);

        // set up the accpetor
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        acceptor_.open(endpoint.protocol()); // == boost::asio::ip::v4();
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));  //  防止进程重启时端口不能快速释放
        acceptor_.bind(endpoint);
        acceptor_.listen();

        acceptor_.async_accept(handler->socket(), [=](boost::system::error_code ec){
            handle_new_connection(handler, ec);// 方便调试
        });


        for (int i = 0; i < thread_count_; ++i)
        {
            thread_pool_.emplace_back( [=]{ io_service_.run(); } );
        }
    }

    void stop_server()
    {
        //io_service_.stop();
        for (std::thread& t : thread_pool_)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }
private:
    void handle_new_connection(shared_hander_t handler, boost::system::error_code const & error)
    {
        if (error)
        {
            return;
        }

        handler->start();

        auto new_handler = std::make_shared<connect_handler>(io_service_);

        acceptor_.async_accept(new_handler->socket(), [=](boost::system::error_code ec){
            handle_new_connection(handler, ec);
        });
    }

    int thread_count_;
    std::vector<std::thread> thread_pool_;
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;

};