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
		const App::Server server{ context };
		std::this_thread::sleep_for (std::chrono::seconds (DELAY_START));

		std::thread t ([&]
			{
				const Network::OnJoined callback = [ ](const Network::TcpCntSharedPtr &connection)
					{
						std::cout << "Client joined: " << connection->GetName () << '\n';
					};

				server.RegisterOnJoinCallback (callback);
				server.Start ();
			});

		while (true)
		{
			std::string buffer;
			std::getline (std::cin, buffer);	//this is blocking until return key is pressed	

			if (buffer == "\\q")
			{
				break;
			}

			buffer += "\n";
			server.Post (std::move(buffer));
		}

		server.Start ();
		t.join ();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what () << '\n';
		return 1;
	}

	return 0;
}
