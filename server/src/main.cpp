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
		io_context localContext;
		App::Server server{ context };
		std::this_thread::sleep_for (std::chrono::seconds (DELAY_START));

		std::thread t ([&]
			{
				Network::OnJoined callback = [ ](const Network::TcpCntSharedPtr &connection)
					{
						std::cout << "Client joined: " << connection->GetName () << '\n';
					};

				server.RegisterOnJoinCallback (callback);
				server.Start ();

				context.run ();
			});

		deadline_timer timer (localContext , boost::posix_time::seconds (5));
		timer.async_wait ([&server ](boost::system::error_code e) { server.Post ("Message sent from server."); });

		// the next two lines are analogous to thread.join ()
		localContext.run ();
		context.run ();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what () << '\n';
		return 1;
	}

	return 0;
}
