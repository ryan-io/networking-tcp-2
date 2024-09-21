#include <iostream>
#include "server.h"
#include "tcpconnection.h"

int main (int charv, char **argv)
{
	using namespace boost::asio;

	try
	{
		constexpr int DELAY_START = 2;
		std::cout << "Creating server...\n";
		io_context context;
		App::Server server{ context };
		std::this_thread::sleep_for (std::chrono::seconds (DELAY_START));

		Network::OnJoined callback = [ ](const Network::TcpCntSharedPtr &connection)
			{
				std::cout << "This is a callback to Client joined: " << connection->GetName() << '\n';
			};

		server.RegisterOnJoinCallback (callback);
		server.Start ();

		context.run ();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what () << '\n';
		return 1;
	}

	return 0;
}
