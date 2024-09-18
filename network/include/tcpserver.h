#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include "tcpconstants.h"

/* 
 * Socket is merely one endpoint of a two-way communication link. It represents a
 * single connection between two entities that are trying to communicate over the
 * network most of the time which are server and client. More than two entities can
 * also be set to communicate but by using multiple sockets.
 *
 * Transport layer often referred to as TCP/IP (Transport Control Protocol/Internet Protocol)
 * For example, when you open your browser and search for something, you’re merely requesting a
 *		server for some information over HTTP (not to forget HTTP is nothing but an application
 *		using TCP/IP service).
 *
 *	Generally speaking, sockets are providing a way for two processes or programs to
 *		communicate over the network
 */

namespace Network {
	using namespace boost::asio;
	using ip::tcp;

	class TcpServer {
		// requires an acceptor to accept incoming connections
		// requires an io_context to handle asynchronous operations
	public:
		static TcpSrvPtr New(io_context& context) {
			return TcpSrvPtr(new TcpServer(context));	// cannot use std::make_shared here
		}

		void DoRun();
		void Run();
		void AcceptHandler(const TcpCntSharedPtr&, const boost::system::error_code&);

		TcpServer(const TcpServer&) = delete;
		TcpServer(TcpServer&&) = delete;
		TcpServer& operator=(const TcpServer&) = delete;
		TcpServer& operator=(TcpServer&&) = delete;
		~TcpServer() = default;

	private:
		// initializes the TcpServer with the provided io_context with the default V4 IP version and port 117
		// we need an acceptor and a context
		// the acceptor requires the context and a new tcp::endpoint with IP version and port
		explicit TcpServer(io_context& context) :
			m_context(context),
			m_acceptor(context,
				tcp::endpoint(tcp::v4(), DEFAULT_TCP_PORT)) {}

		static void Log(const char* message) { std::cout << message << '\n'; }

		std::vector<TcpCntSharedPtr> m_connections{};
		io_context& m_context;
		tcp::acceptor m_acceptor;
	};
}