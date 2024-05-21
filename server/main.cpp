#include <iostream>
#include <cstdint>
#include <vector>
#include <string>

#include "server/Server.hpp"
#include "controller/controller.hpp"
#include "misc/globals.hpp"

Server server;

int main(int argc, char **argv)
{
	server.onConnect(&Controller::handleServerConnection);
	server.onDisconnect(&Controller::handleServerDisconnect);
	server.onInput(&Controller::handleServerInput);
	server.init();

	while (true)
	{
		server.loop();
	}
}
