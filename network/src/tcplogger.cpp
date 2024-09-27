#include "tcplogging.h"

namespace Network
{
	void TcpLogging::LogMsg (char *msg) const
	{
		*m_ostream << msg << '\n';
	}

	void TcpLogging::LogErr (char *msg) const
	{
		*m_ostream << msg << '\n';
	}

	void TcpLogging::LogMsg (const std::string &msg) const
	{
		*m_ostream << msg << '\n';
	}

	void TcpLogging::LogErr (const std::string &msg) const
	{
		*m_ostream << msg << '\n';
	}

	TcpLogging::~TcpLogging ()
	{
		delete m_ostream;
	}
}