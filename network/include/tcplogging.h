#pragma once

namespace Network
{
	// intended to be used as a singleton
	class TcpLogging
	{
	public:
		virtual void LogMsg (char *) const = 0;
		virtual void LogErr (char *) const = 0;
		virtual void LogMsg (const std::string &msg) const = 0;
		virtual void LogErr (const std::string &msg) const = 0;
		virtual ~TcpLogging ();

		explicit TcpLogging (std::ostream *ostream) : m_ostream (ostream) { }

		TcpLogging (const TcpLogging &) = delete;				
		TcpLogging (TcpLogging &&) = delete;					// allow move semantics?
		TcpLogging &operator= (const TcpLogging &) = delete;
		TcpLogging &operator= (TcpLogging &&) = delete;			// allow move semantics?

		/// <summary>
		/// Overloaded '<<' operator that internally creates an std::ostringstream
		/// object, writes it to the stream and then logs the message via 'LogMsg' implementation.
		/// </summary>
		template<typename Tobj>
		friend std::ostream &operator<< (std::ostream &out, const Tobj &obj)
		{
			std::ostringstream oss;
			oss << obj;
			out << oss.str ();
			return out;
		}

		std::ostream *m_ostream;
	};
}