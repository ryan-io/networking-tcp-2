#pragma once

#include <numeric>
#include <boost/asio.hpp>
#include "tcpconstants.h"

namespace Network {
	using namespace boost::asio;
	using ip::tcp;

	struct TcpClient {
		static TcpClientPtr New(io_context& context) {
			return TcpClientPtr(new TcpClient(context));
		}

		~TcpClient() = default;

		void open();

	private:
		explicit TcpClient(io_context& context) : m_context(context), m_socket(context) {
			auto resolver = tcp::resolver(context);
			auto endpoints = resolver.resolve(
				DEFAULT_ADDRESS,
				DEFAULT_TCP_PORT_STR);
			m_endpoints = std::move(endpoints);
		}

		bool m_isOpen = false;
		io_context& m_context;
		tcp::socket m_socket;
		ip::basic_resolver_results<tcp> m_endpoints;
	};
}