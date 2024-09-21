#pragma once

#include <boost/asio.hpp>
#include "tcpconstants.h"

namespace Network
{
	using namespace boost::asio;
	using ip::tcp;

	class TcpClient
	{
		using Messaged = std::function<void (const std::string &)>;
	public:
		// static factory method for creating a new TcpClient object
		static TcpClientPtr New (io_context &context);

		// default constructor
		~TcpClient ();

		void Open ();

	private:
		struct TcpClientImpl;
		std::unique_ptr<TcpClientImpl> m_impl;

		explicit TcpClient (io_context &context);
	};
}  