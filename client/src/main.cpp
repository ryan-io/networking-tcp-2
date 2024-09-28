#include <iostream>
#include <boost/asio/io_context.hpp>
#include "tcpclient.h"
#include "getio.h"

Network::TcpClientPtr client;

int main (int charv, char **argv)
{
	using namespace std::chrono_literals;

	try
	{
		// ideally, a new client will be created with command line argument sin 'argv'
		// that contain the host address and port number to connect to.
		//const Network::TcpLogger logger = Network::TcpLogger(&std::cout);

		const auto log = Network::TcpLogging{ &std::cout };

		client = Network::TcpClient::New ("127.0.0.1", "117", 1024, &log);

		std::cout << "Client started!\n";

		auto clientThread = client->OpenThread ();

		Network::GetInput ([ ](std::string &in) { client->Post (std::move (in)); });	// get input from user

		client->Close ();
		clientThread.join ();
	}
	catch (const std::exception &e)
	{
		// error handling
		std::cerr << e.what () << std::endl;
	}
	return 0;
}
