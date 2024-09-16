#pragma once

namespace Network {
	constexpr int DEFAULT_BUFFER_SIZE = 1024;
	constexpr char DEFAULT_TCP_PORT_STR[] = "117";
	constexpr unsigned short DEFAULT_TCP_PORT = 117;	// network ports are 2 bytes, 0 to 65535
	constexpr char DEFAULT_ADDRESS[] = "127.0.0.1";	// localhost; this line requires C++17

	struct TcpConnection;
	struct TcpClient;
	class TcpServer;

	using TcpClientPtr = std::unique_ptr<TcpClient>;
	using TcpClientSharedPtr = std::shared_ptr<TcpClient>;
	using TcpCntPtr = std::unique_ptr<TcpConnection>;
	using TcpCntSharedPtr = std::shared_ptr<TcpConnection>;
	using TcpSrvPtr = std::unique_ptr<TcpServer>;
	using TcpSrvSharedPtr = std::shared_ptr<TcpServer>;
}