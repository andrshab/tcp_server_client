#include <iostream>
#include <cstring>
#include <map>

#include "controller.hpp"
#include "../parser/parser.hpp"
#include "../misc/globals.hpp"
#include "../misc/map_get.hpp"

using namespace std;

Parser prs;
map<string,Controller::handler> handlers;
int connectionsNumber = 0;

void Controller::handleServerConnection(uint16_t fd)
{
	cout << "Controller: got connect from " << fd << endl;
	connectionsNumber++;
}

void Controller::handleServerDisconnect(uint16_t fd)
{
	cout << "Controller: got disconnect from " << fd << endl;
	connectionsNumber--;
}

void Controller::handleServerInput(uint16_t fd, char *buffer)
{
	cout << "Controller: got input '" << buffer << "'from " << fd << endl;
	prs.parse(buffer);
	for(string i : prs.values)
	{
		cout << "Controller::handleServerInput -> for: input: '" << i << "'.\n";
	}


	//get the first argument name
	string key;
	try
	{
		key = prs.values.at(0);
	}
	catch(out_of_range oor)
	{
		cerr << "no action" << endl;
	}

	//look it up in the handlers map
	Controller::handler h = NULL;

	char buf[100];
	if (key == "connectionsNumber") {
		sprintf(buf, "connectionsNumber: %d", connectionsNumber);
	} else if (key == "parse" && prs.values.size() == 2) {
		sprintf(buf, "parsedString: %d", prs.values.at(1).size());
	} else {
		sprintf(buf, "not valid request");
	}
	
	send(fd, buf, strlen(buf), 0);

	prs.values.clear();
}

void Controller::addHandler(string key, Controller::handler cb)
{
	handlers.insert(pair<string,Controller::handler>(key,cb));
}

void Controller::removeHandler(string key)
{
	cout << "Controller::removeHandler: key=" << key << endl;
}
