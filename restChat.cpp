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
map<string, string> credentials;

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




void handle_register(const Request& req, Response& res) {
  // Extract the user credentials from the request URL parameters
  string name = req.get_param_value("name");
  string email = req.get_param_value("email");
  string password = req.get_param_value("password");

  // Validate the user credentials
  if (name.empty() || email.empty() || password.empty()) {
    res.status = 400;
    res.set_content("{\"error\":\"Please fill out all fields\"}", "application/json");
    return;
  } else if (credentials.count(email) > 0) {
    res.status = 400;
    res.set_content("{\"error\":\"Email address already registered\"}", "application/json");
    return;
  } else if (password.length() < 6) {
    res.status = 400;
    res.set_content("{\"error\":\"Password must be at least 6 characters long\"}", "application/json");
    return;
  }

  // Store the user credentials in memory
  credentials[email] = password;

  // Send the response
  res.set_content("{\"status\":\"success\"}", "application/json");
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


  svr.Get("/chat/register/:name/:email/:password", [&](const Request& req, Response& res) {
    handle_register(req, res);
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
		
	}
    res.set_content(resultMessage, "text/json");
    res.set_content(resultUser, "text/json");
  });
  
   svr.Get(R"(/chat/fetch/(.*))", [&](const Request& req, Response& res) {
    string username = req.matches[1];
    res.set_header("Access-Control-Allow-Origin","*");
    string resultMessageJSON = getMessagesJSON(username,messageMap);
    res.set_content(resultMessageJSON, "text/json");
 
  });
  
  cout << "Server listening on port " << port << endl;
  svr.listen("0.0.0.0", port);

}
