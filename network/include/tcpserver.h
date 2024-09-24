#pragma once

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

namespace Network
{
	using OnJoined = std::function<void (const TcpCntSharedPtr &)>;
	using OnLeft = std::function<void (const TcpCntSharedPtr &)>;

	using namespace boost::asio;
	using ip::tcp;

	// requires an acceptor to accept incoming connections
	// requires an io_context to handle asynchronous operations
	class TcpServer
	{
	public:
		// static factory method to create a new TcpServer object
		// requires the consumer to provide a boost io_context
		static TcpSrvPtr New (io_context &context);

		// enables the server to start accepting connections
		// if this method is not invoked, no connections will be accepted
		//		and the server will not be able to broadcast messages
		void Start ();

		// send a message to all connected clients
		void Broadcast (const std::string &msg) const;

		// register a callback for when a client joins
		void RegisterOnJoin (const OnJoined &onJoined) const;
		void Post(const char* msg) const;

#pragma region Construction/Destruction

		TcpServer (const TcpServer &) = delete;				// delete copy constructor
		TcpServer (TcpServer &&) = delete;					// delete move constructor
		TcpServer &operator=(const TcpServer &) = delete;	// delete copy assignment
		TcpServer &operator=(TcpServer &&) = delete;		// delete move assignment

		// only declare destructor here, implementation is in the cpp file
		// this is to allow PIMPL idiom to work with unique_ptr and invocation of tcp server destructor
		~TcpServer ();

	#pragma endregion

	private:
		struct TcpServerImpl;	// forward declaration of the implementation (PIMPL)
		std::unique_ptr<TcpServerImpl> m_impl;	// pointer to the implementation

		// the core function that processes connection loop
		void Loop ();

		void RelayOnJoin (const TcpCntSharedPtr &) const;
		void RelayOnLeft (const TcpCntSharedPtr &) const;

		// initializes the TcpServer with the provided io_context with the default V4 IP version and port 117
		// we need an acceptor and a context
		// the acceptor requires the context and a new tcp::endpoint with IP version and port
		explicit TcpServer (io_context &context);

		// static method for wrapping std::cout
		static void Log (const char *message);
	};
}