#include <iostream>
#include <vector>

#include <ecs/ecs.h>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>

#include "ThreadPool.h"

int main(int argc, char** argv)
{
	ThreadPool threadPool;

	boost::asio::ip::tcp::acceptor acceptor(threadPool.ioService);
	boost::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(threadPool.ioService));

	boost::asio::ip::tcp::resolver resolver(threadPool.ioService);
	boost::asio::ip::tcp::resolver::query query(
		"127.0.0.1",
		"7777"
		);

	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
	acceptor.bind(endpoint);
	acceptor.listen(boost::asio::socket_base::max_connections);
	std::cout << "listening on: " << endpoint << std::endl;
	acceptor.async_accept(*sock, [](const boost::system::error_code& ec){std::cout << "accepted" << std::endl; });

	std::cin.get();

	char* buffer = new char[60];
	
	int amount = sock->read_some(boost::asio::buffer(buffer,60));

	std::cin.get();

	acceptor.close();
	sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	sock->close();
}