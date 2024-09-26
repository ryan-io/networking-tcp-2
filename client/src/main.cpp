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
		const auto client = Network::TcpClient::New ("127.0.0.1", "117");
		std::cout << "Client started!\n";

		// new thread for client to read/write messages
		std::thread t{
			[&client] ()
			{
				client->Open ();
			}
		};

		while (true)
		{
			std::string buffer;
			std::getline (std::cin, buffer);	//this is blocking until return key is pressed	

			if (buffer == "\\q")
			{
				break;
			}

			buffer += "\n";
			client->Post (std::move (buffer));
		}

		client->Close ();
		t.join ();
	}
	catch (const std::exception &e)
	{
		// error handling
		std::cerr << e.what () << std::endl;
	}
	return 0;
}
