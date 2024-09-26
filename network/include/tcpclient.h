#pragma once

#include <boost/asio.hpp>
#include "tcpconstants.h"
#include "tcplogging.h"

namespace Network
{
	using namespace boost::asio;
	using ip::tcp;

	class TcpClient
	{
	public:
		// static factory method for creating a new TcpClient object
		static TcpClientPtr New (io_context &context);

		// default constructor
		~TcpClient ();

		void Open ();
		void InternalLogMsg(const std::string&) const;
		void InternalLogMsg(const char*) const;
		void InternalLogErr (const std::string &) const;
		void InternalLogErr (const char *) const;
		void Close () const;
		void Post (std::string &&message) const;

	private:
		[[nodiscard]] bool IsOpen () const;
		void SetOpen () const;
		void SetClose () const;
		void AsyncRead () const;
		void AsyncWrite ();

		struct TcpClientImpl;
		std::unique_ptr<TcpClientImpl> m_impl;

		explicit TcpClient (io_context &, const TcpLogging *logging = nullptr);
	};
}  