#include <iostream>

#include "client.h"

int main (int charv, char **argv)
{
	using namespace std::chrono_literals;
	try
	{
		boost::asio::io_context context;
		const auto client = App::Client::New (context);
		std::cout << "Client started!\n";

		// new thread for client to read/write messages
		std::thread t{
			[&client] ()
			{
				client.Start ();
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
			client.Post (std::move (buffer));
		}

		client.Stop ();
		t.join ();
	}
	catch (const std::exception &e)
	{
		// error handling
		std::cerr << e.what () << std::endl;
	}
	return 0;
}
