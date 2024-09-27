#include <iostream>

#include "tcpconnection.h"
#include "tcpserver.h"

int main (int charv, char **argv)
{
	using namespace boost::asio;

	try
	{
		std::cout << "Creating server...\n";

		const auto log = Network::TcpLogging{&std::cout};
		const auto server = Network::TcpServer::New (117, 1024, &log);

		const Network::OnJoin callback = [ ](const Network::TcpCntSharedPtr &connection)
			{
				std::cout << "Client joined: " << connection->GetName () << '\n';
			};

		server->RegisterOnJoin (callback);

		auto t = server->StartThread ();

		while (true)
		{
			std::string buffer;
			std::getline (std::cin, buffer);	//this is blocking until return key is pressed	

			if (buffer == "\\q")
			{
				break;
			}

			buffer += "\n";
			server->Post (std::move (buffer));
		}

		server->Stop ();
		t.join ();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what () << '\n';
		return 1;
	}

	return 0;
}
