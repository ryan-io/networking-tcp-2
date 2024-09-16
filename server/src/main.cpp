#include <iostream>
#include "server.h"

int main(int charv, char** argv) {
	using namespace boost::asio;

	try {
		constexpr int DELAY_START = 2;
		std::cout << "Creating server...\n";
		io_context context;
		const App::Server server{ context };

		std::this_thread::sleep_for(std::chrono::seconds(DELAY_START));
		server.Start();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}

	return 0;
}