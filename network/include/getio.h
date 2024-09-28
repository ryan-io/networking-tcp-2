#pragma once

#include <iostream>

namespace Network
{
	static void GetInput (const std::function<void(std::string& in)>& callback)
	{
		std::cin.clear ();

		while (true)
		{
			std::string buffer;
			std::getline (std::cin, buffer);	//this is blocking until return key is pressed	

			if (buffer == "\\q")
			{
				break;
			}

			buffer.push_back('\n');
			callback (buffer);
			buffer.clear ();
			std::cin.clear ();
		}
	}
}
