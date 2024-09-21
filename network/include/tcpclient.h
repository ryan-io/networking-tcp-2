#pragma once

#include <boost/asio.hpp>
#include "tcpconstants.h"

namespace Network
{
	using namespace boost::asio;
	using ip::tcp;

	struct TcpClient
	{
		// static factory method for creating a new TcpClient object
		static TcpClientPtr New (io_context &context);

		// default constructor
		~TcpClient () = default;

		void Open ();

	private:
		explicit TcpClient (io_context &context);

		bool m_isOpen = false;
		io_context &m_context;
		tcp::socket m_socket;
		ip::basic_resolver_results<tcp> m_endpoints;
	};
}  