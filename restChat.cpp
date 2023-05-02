//
//  namesAPI.cc - a microservice demo program
//
// James Skon
// Kenyon College, 2022
//

#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include "httplib.h"

using namespace httplib;
using namespace std;

const int port = 5005;

void addMessage(string username, string message, map<string,vector<string>> &messageMap) {
	/* iterate through users adding message to each */
	string jsonMessage = "{\"user\":\""+username+"\",\"message\":\""+message+"\"}";
	for (auto userMessagePair : messageMap) {
		username = userMessagePair.first;
		messageMap[username].push_back(jsonMessage);
	}
}

// Return messages for a certain user
string getMessagesJSON(string username, map<string,vector<string>> &messageMap) {
	/* retrieve json list of messages for this user */
	bool first = true;
	string result = "{\"messages\":[";
	for (string message :  messageMap[username]) {
		if (not first) result += ",";
		result += message;
		first = false;
	}
	result += "]}";
	messageMap[username].clear();
	return result;
}

// My function to get list of users
void addUsers(string username, map<string,vector<string>> &messageMap, vector<string> userList) {
	// Create a json list of current users
	string jsonUser = "{\"user\":\""+username+"\"}";
	for (auto userMessagePair : messageMap) {
		username = userMessagePair.first;
		userList[username].push_back(jsonUser);
	}
}

// Return a json list of user 
string getUsersJSON(string username, vector<string> &userList) {
	/* retrieve json list of users */
	bool first = true;
	string result = "{\"user\":[";
	for (string user :  userList[username]) {
		if (not first) result += ",";
		result += user;
		first = false;
	}
	result += "]}";
	userList[username].clear();
	return result;
}

int main(void) {
  Server svr;
  int nextUser=0;
  map<string,vector<string>> messageMap;
  vector<string> userList;
	
  /* "/" just returnsAPI name */
  svr.Get("/", [](const Request & /*req*/, Response &res) {
    res.set_header("Access-Control-Allow-Origin","*");
    res.set_content("Chat API", "text/plain");
  });


  svr.Get(R"(/chat/join/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
    string username = req.matches[1];
    string resultMessage;
    string resultUser;
    vector<string> empty;
    cout << username << " joins" << endl;
    
    // Check if user with this name exists
    if (messageMap.count(username)) {
    	resultUser = "{\"status\":\"exists\"}";
    } else {
    	// Add user to messages map
    	messageMap[username]=empty;
    	resultMessage = "{\"status\":\"success\",\"user\":\"" + username + "\"}";
    	// Add user to user list
    	userList[username]=empty;
    	resultUser = "{\"status\":\"success\",\"user\":\"" + username + "\"}";
    }
    res.set_content(resultMessage, "text/json");
    res.set_content(resultUser, "text/json");
  });

   svr.Get(R"(/chat/send/(.*)/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
	string username = req.matches[1];
	string message = req.matches[2];
	string resultMessage; 
	string resultUser; 
	
    if (!messageMap.count(username)) {
    	resultUser = "{\"status\":\"baduser\"}";
	} else {
		addMessage(username,message,messageMap);
		resultMessage = "{\"status\":\"success\"}";
		addUsers(username,messageMap,userList);
		resultUser = "{\"status\":\"success\"}";
	}
    res.set_content(resultMessage, "text/json");
    res.set_content(resultUser, "text/json");
  });
  
   svr.Get(R"(/chat/fetch/(.*))", [&](const Request& req, Response& res) {
    string username = req.matches[1];
    res.set_header("Access-Control-Allow-Origin","*");
    string resultMessageJSON = getMessagesJSON(username,messageMap);
    res.set_content(resultMessageJSON, "text/json");
    string resultUsersJSON = getUsersJSON(username,userList);
    res.set_content(resultUsersJSON, "text/json");
  });
  
  cout << "Server listening on port " << port << endl;
  svr.listen("0.0.0.0", port);

}
