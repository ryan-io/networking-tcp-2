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
#pragma region TYPEDEFS

	/// <summary>
	///	A callback that is emitted when a new client joins the server
	/// </summary>
	using OnJoin = std::function<void (const TcpCntSharedPtr &)>;

	/// <summary>
	///	A callback that is emitted when a new client joins the server
	/// </summary>
	using OnLeave = std::function<void (const TcpCntSharedPtr &)>;

	using namespace boost::asio;
	using ip::tcp;

#pragma endregion

	// An abstraction for handling communication between two entities (server and client)
	class TcpServer
	{
	public:
	#pragma region FUNCTIONS
		/// <summary>
		/// Static factory method for creating a new TcpServer object.
		/// </summary>
		static TcpSrvPtr New (io_context &context);

		/// <summary>
		/// Starts the server for communication; sets 'IsRunning' to true.
		/// </summary>
		void Start ();

		/// <summary>
		/// Stops the server for communication; sets 'IsRunning' to false.
		/// </summary>
		void Stop () const;

		/// <summary>
		/// Registers to the 'OnJoin' callback for when a new client joins the server. 
		/// </summary>
		void RegisterOnJoin (const OnJoin &) const;

			/// <summary>
		/// Registers to the 'OnJoin' callback for when a new client joins the server. 
		/// </summary>
		void RegisterOnLeave (const OnLeave &) const;

		/// <summary>
		/// Send a message to all connected clients.
		/// </summary>
		void Post (std::string &&msg) const;

		TcpServer (const TcpServer &) = delete;				// delete copy constructor
		TcpServer (TcpServer &&) = delete;					// delete move constructor
		TcpServer &operator=(const TcpServer &) = delete;	// delete copy assignment
		TcpServer &operator=(TcpServer &&) = delete;		// delete move assignment

		//// <summary>
		/// Default destructor.
		/// </summary>
		~TcpServer ();

	#pragma endregion

	private:
	#pragma region MEMBER_VARIABLES
		/// <summary>
		/// PIMPL idiom for handling the implementation details of TcpServer.
		/// </summary>
		struct TcpServerImpl;	// forward declaration of the implementation (PIMPL)

		/// <summary>
		/// Unique pointer to PIMPL implementation.
		/// </summary>
		std::unique_ptr<TcpServerImpl> m_impl;	// pointer to the implementation

	#pragma endregion

	#pragma region FUNCTIONS
		/// <summary>
		/// Core async read/write loop for handling communication.
		/// </summary>
		void Loop ();

		/// <summary>
		/// Invokes all subscribers when a client joins a server
		/// </summary>
		void RelayOnJoin (const TcpCntSharedPtr &) const;

		/// <summary>
		/// Invokes all subscribers when a client leaves a server
		/// </summary>
		void RelayOnLeft (const TcpCntSharedPtr &) const;

		/// <summary>
		/// Default constructor; initializes the TcpServer object.
		/// </summary>
		explicit TcpServer (io_context &context);

		/// <summary>
		///	Checks if a logger is provided and logs the message.
		/// </summary>
		void InternalLogMsg (const std::string &) const;
		void InternalLogMsg (const char *) const;

		/// <summary>
		///	Checks if a logger is provided and logs the message as an error.
		/// </summary>
		void InternalLogErr (const std::string &) const;
		void InternalLogErr (const char *) const;

	#pragma endregion
	};
}