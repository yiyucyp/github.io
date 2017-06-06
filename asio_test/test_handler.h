#pragma once

#include <boost/asio.hpp>

class test_handler
    : public std::enable_shared_from_this<test_handler>
{
public:
    test_handler(boost::asio::io_service& service)
        : io_service_(service)
        , socket_(service)
        , write_strand_(service)
    {

    }

    void start()
    {
        read_packet();
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return socket_;
    }

    void send_data(void* data, std::size_t len)
    {
        //socket_.async_send();
        //std::string str((char*)data);
        io_service_.post(write_strand_.wrap(std::bind(&test_handler::do_write, shared_from_this())));//确保串行执行
    }

private:
    void read_packet()
    {
        //shared_from_this();


        socket_.async_receive(boost::asio::buffer((void*)ary_receive_, sizeof(ary_receive_)), std::bind(&test_handler::on_receive, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        //已经完整读出，或是出错
        if (error)
        {
            return;
        }

        ary_receive_[bytes_transferred - 1] = 0;
        std::string str_pack = (char*)ary_receive_;
        memcpy(ary_send_, ary_receive_, sizeof(ary_send_));

        send_data((void*)str_pack.c_str(), str_pack.length());

        read_packet();

    }

    // 这个内存要一直有效，直接发送完毕
    void do_write()
    {
        socket_.async_send(boost::asio::buffer(ary_send_), std::bind(&test_handler::on_send, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void on_send(const boost::system::error_code& error, std::size_t bytes_transferred)
    {

    }


    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_service::strand write_strand_;
    uint8_t ary_receive_[256];
    uint8_t ary_send_[256];
};