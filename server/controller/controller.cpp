#include <iostream>
#include <cstring>
#include <map>

#include "controller.hpp"
#include "../parser/parser.hpp"
#include "../misc/globals.hpp"
#include "../misc/map_get.hpp"

using namespace std;

struct MsgData {
	uint16_t receiver;
	uint16_t sender;
	string msg;
};

Parser prs;
map<string,Controller::handler> handlers;
int connectionsNumber = 0;
vector<MsgData> messagesDataBase;

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

void getMessage(uint16_t fd, uint16_t &senderFd, string &msg) {
	int postion = -1;
	int idx = 0;
	for (const MsgData msg_data : messagesDataBase) {
		if (msg_data.receiver == fd) {
			msg = msg_data.msg;
			senderFd = msg_data.sender;
			postion = idx;
			break;
		}
		idx++;
	}
    	
	if (postion >= 0) {
		messagesDataBase.erase(messagesDataBase.begin() + postion);
	}
}

string getStringStats(string input) {
	string result = "";
	uint16_t countLetters[100] = {0};
	for (int i = 0; i < input.length(); i++) {
		char c = input[i];
		if (c < 'A' || c > 'z') continue;
		countLetters[c-'A'] += 1;
	}
	for (int i = 0; i < input.length(); i++) {
		char c = input[i];
		result = result + input[i] + to_string(countLetters[c-'A']);	
	}
	return result;
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
		sprintf(buf, "parsedString: %s", getStringStats(prs.values.at(1)).c_str());
	} else if (key == "put" && prs.values.size() == 3) {
		uint16_t recieverFd = stoi(prs.values.at(1));
		MsgData msgData;
		msgData.msg = prs.values.at(2);
		msgData.sender = fd;
		msgData.receiver  = recieverFd;
		messagesDataBase.push_back(msgData);
		sprintf(buf, "message sent to %d", recieverFd);
	} else if (key == "get") {
		// uint16_t senderFd = stoi(prs.values.at(1));
		uint16_t senderFd = 0;
		string msg = "";
		getMessage(fd, senderFd, msg);
		if (!msg.empty()) {
			sprintf(buf, "new msg from %d: %s", senderFd, msg.c_str());
		} else {
			sprintf(buf, "no messages");
		}
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
