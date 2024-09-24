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

		std::thread t{
			[&client] ()
			{
				client.Start ();
			}
		};

		int counter = 0;
		while (true)
		{
			/*if (counter >= 5)
			{
				break;
			}*/

			std::this_thread::sleep_for (1000ms);	// std::chrono_literals
			client.Post ("Hello from client!");
			std::cout << "Client is still open\n";
			++counter;
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
