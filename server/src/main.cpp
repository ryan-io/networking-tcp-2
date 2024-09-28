#include <iostream>

#include "tcpconnection.h"
#include "tcpserver.h"
#include "getio.h"

Network::TcpSrvPtr server;

int main (int charv, char **argv)
{
	using namespace boost::asio;

	try
	{
		std::cout << "Creating server...\n";

		const auto log = Network::TcpLogging{ &std::cout };
		server = Network::TcpServer::New (117, 1024, &log);

		const Network::OnJoin callback = [ ](const Network::TcpCntSharedPtr &connection)
			{
				std::cout << "Client joined: " << connection->GetName () << '\n';
			};

		server->RegisterOnJoin (callback);

		auto t = server->StartThread ();

		Network::GetInput ([ ](std::string &in) { server->Post (std::move (in)); });	// get input from user

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
