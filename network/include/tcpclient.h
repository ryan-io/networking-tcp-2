#pragma once

#include <boost/asio.hpp>
#include "tcpconstants.h"
#include "tcplogging.h"

namespace Network
{
#pragma region TYPEDEFS

	using namespace boost::asio;
	using ip::tcp;

#pragma endregion

	class TcpClient
	{
	public:
	#pragma region FUNCTIONS
		// Static factory method for creating a new TcpClient object.
		static TcpClientPtr New (string_view host, string_view port, int bufferSize = 1024, const TcpLogging *logger = nullptr);

		// Default destructor.
		~TcpClient ();

		/// <summary>
		///	Opens the client for communication; sets 'IsOpen' to true.
		/// </summary>
		void Open () const;

		/// <summary>
		///	Opens the client for communication; sets 'IsOpen' to false.
		/// </summary>
		void Close () const;

		/// <summary>
		/// Asynchronously writes to the server with the provided address & port.
		/// </summary>
		void Post (std::string &&message) const;

		/// <summary>
		/// Copy and move assignment operators are deleted.
		/// </summary>
		TcpClient &operator= (const TcpClient &) = delete;
		TcpClient &operator= (TcpClient &&) = delete;

	#pragma endregion

	private:
	#pragma region MEMBER_VARIABLES

		/// <summary>
		///	PIMPL idiom for handling the implementation details of TcpClient.
		/// </summary>
		struct TcpClientImpl;
		std::unique_ptr<TcpClientImpl> m_impl;

		/// <summary>
		///	Returns true if the client is open for communication, otherwise false.
		/// </summary>
		[[nodiscard]] bool IsOpen () const;

	#pragma endregion

	#pragma region FUNCTIONS
		/// <summary>
		/// Default constructor; initializes the TcpClient object.
		///	Pass a host address, the port number, buffer size and an optional logger.
		/// </summary>
		explicit TcpClient (string_view host, string_view port, int bufferSize, const TcpLogging *logger = nullptr);

		/// <summary>
		/// Sets the client to open for communication.
		/// </summary>
		void SetOpen () const;

		/// <summary>
		/// Sets the client to open for communication.
		/// </summary>
		void SetClose () const;

		/// <summary>
		/// Writes to the server asynchronously with the provided address & port.
		/// </summary>
		void AsyncRead () const;

		/// <summary>
		/// Reads to the server asynchronously with the provided address & port.
		/// </summary
		void AsyncWrite ();

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

		/// <summary>
		/// Copy and move constructors are deleted.
		/// </summary>
		TcpClient (const TcpClient &) = delete;
		TcpClient (TcpClient &&) = delete;

	#pragma endregion
	};
}