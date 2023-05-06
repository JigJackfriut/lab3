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
#include <string>
#include "httplib.h"
#include <vector>

#include "restDB.h"
#include "restEntry.h"

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






int main(void) {
  Server svr;
  restDB rDB 
  int nextUser=0;
  map<string,vector<string>> messageMap;
  vector<string> userList;
	
  /* "/" just returnsAPI name */
  svr.Get("/", [](const Request & /*req*/, Response &res) {
    res.set_header("Access-Control-Allow-Origin","*");
    res.set_content("Chat API", "text/plain");
  });

	
//code for registration!!!!!!!
svr.Get(R"(/chat/register/(.*)/(.*)/(.*))", [&](const Request& req, Response& res) {
    // Extract the name, email, and password from the request URL
    res.set_header("Access-Control-Allow-Origin","*");
    cout<<"hello";
    std::string name = req.matches[1];
    std::string email = req.matches[2];
    std::string password = req.matches[3];

    // Check if the username is already taken
    if (userCredentials.count(name)) {
      // Return an error response
      res.set_content("{\"error\":\"Username already taken\"}", "application/json");
    } else {
      // Register the new user
      userCredentials[name] = password;
      userCredentialsnow[name]=password;

      // Return a success response
      res.set_content("{\"message\":\"User registered successfully\"}", "application/json");
    }
  });
	
svr.Get(R"(/chat/login/(.*)/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
    std::string name = req.matches[1];
    std::string password = req.matches[2];

    if (userCredentials.count(name) && userCredentials[name] == password) {
	userCredentialsnow[name]=password;
        res.set_content("{\"message\":\"User logged in successfully\"}", "application/json");
    } else {
        res.set_content("{\"error\":\"Invalid username or password\"}", "application/json");
    }
});
	

 // Send message endpoint
svr.Get(R"(/chat/send/(.*)/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
    string username = req.matches[1];
    string message = req.matches[2];
    string resultMessage;

    if (!userCredentials.count(username)) {
        resultMessage = "{\"status\":\"baduser\"}";
    } else {
        addMessage(username, message, messageMap);
        resultMessage = "{\"status\":\"success\"}";
    }

    res.set_content(resultMessage + ",\"user\":\"" + username + "\"}", "application/json");
});
  
   svr.Get(R"(/chat/fetch/(.*))", [&](const Request& req, Response& res) {
    string username = req.matches[1];
    res.set_header("Access-Control-Allow-Origin","*");
    string resultMessageJSON = getMessagesJSON(username,messageMap);
    res.set_content(resultMessageJSON, "text/json");
 
  });
  
   svr.Get(R"(/chat/list)", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
	string result;
	string usernameList = "Users: ";
    for (auto const &pair:userCredentialsnow ) {
		usernameList += pair.first;
		usernameList += ", ";
    }
	string jsonMessage = "{\"userList\":\""+usernameList+"\"}";
	res.set_content(jsonMessage, "text/json");
  });
	
	
   svr.Get(R"(/chat/logout/(.*))", [&](const Request& req, Response& res) {
    
    res.set_header("Access-Control-Allow-Origin","*");
	   string username = req.matches[1];
	   userCredentialsnow.erase(username); 
  });	
	
	
  cout << "Server listening on port " << port << endl;
  svr.listen("0.0.0.0", port);

}
