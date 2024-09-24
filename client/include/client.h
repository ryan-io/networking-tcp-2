#pragma once
#include "tcpclient.h"

namespace App {
	class Client {
	public:
		static Client New(boost::asio::io_context& context) {
			return Client(context);
		}

		void Start() const;
		void Stop () const;
		void Post (std::string &&msg) const;

	private:
		explicit Client(boost::asio::io_context& context) :
			m_tcpClient(Network::TcpClient::New(context)),
			m_context(context){}

		Network::TcpClientPtr m_tcpClient;
		boost::asio::io_context& m_context;
	};
}