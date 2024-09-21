#include <iostream>

#include "client.h"

int main (int charv, char **argv)
{
	try
	{
		boost::asio::io_context context;
		const auto client = App::Client::New (context);
		std::cout << "Client started!\n";
		client.Start ();
		context.run ();
	}
	catch (const std::exception &e)
	{
		// error handling
		std::cerr << e.what () << std::endl;
	}
	return 0;
}
