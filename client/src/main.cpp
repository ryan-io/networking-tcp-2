#include <iostream>
#include <boost/asio/io_context.hpp>
#include "tcpclient.h"

int main (int charv, char **argv)
{
	using namespace std::chrono_literals;

	try
	{
		// ideally, a new client will be created with command line argument sin 'argv'
		// that contain the host address and port number to connect to.
		//const Network::TcpLogger logger = Network::TcpLogger(&std::cout);

		const auto log = Network::TcpLogging{ &std::cout };

		const auto client = Network::TcpClient::New ("127.0.0.1", "117", 1024, &log);

		std::cout << "Client started!\n";

		auto clientThread = client->OpenThread ();

		while (true)
		{
			std::string buffer;
			std::getline (std::cin, buffer);	//this is blocking until return key is pressed	

			if (buffer == "\\q")
			{
				break;
			}

			buffer += "\n";
			client->Post (std::move (buffer));	// callback
			buffer.clear ();
			std::cin.clear ();
		}

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
