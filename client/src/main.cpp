#include <cstdlib>
#include "client.h"

int main(int charv, char** argv) {
	boost::asio::io_context context;
	const auto client = App::Client::New(context);
	client.Start();
	system("pause");
	return 0;
}
