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
  restDB rDB;
  map<string,vector<string>> messageMap;

	
  /* "/" just returnsAPI name */
  svr.Get("/", [](const Request & /*req*/, Response &res) {
    res.set_header("Access-Control-Allow-Origin","*");
    res.set_content("Chat API", "text/plain");
  });
	
	
	
	
	
	
	svr.Get(R"(/chat/join/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
    string username = req.matches[1];
    string result;
    vector<string> empty;
    cout << username << " joins" << endl;
    
    // Check if user with this name exists
    if (messageMap.count(username)) {
    	result = "{\"status\":\"exists\"}";
    } else {
    	// Add user to messages map
    	messageMap[username]=empty;
    	result = "{\"status\":\"success\",\"user\":\"" + username + "\"}";
    }
    res.set_content(result, "text/json");
  });

   svr.Get(R"(/chat/send/(.*)/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
	string username = req.matches[1];
	string message = req.matches[2];
	string result; 
	
    if (!messageMap.count(username)) {
    	result = "{\"status\":\"baduser\"}";
	} else {
		addMessage(username,message,messageMap);
		result = "{\"status\":\"success\"}";
	}
    res.set_content(result, "text/json");
  });
  
   svr.Get(R"(/chat/fetch/(.*))", [&](const Request& req, Response& res) {
    string username = req.matches[1];
    res.set_header("Access-Control-Allow-Origin","*");
    string resultJSON = getMessagesJSON(username,messageMap);
    res.set_content(resultJSON, "text/json");
  });

	
//code for registration!!!!!!!
svr.Get(R"(/chat/register/(.*)/(.*)/(.*))", [&](const Request& req, Response& res) {
    // Extract the name, email, and password from the request URL
    res.set_header("Access-Control-Allow-Origin","*");
    std::string name = req.matches[1];
    std::string email = req.matches[2];
    std::string password = req.matches[3];

    // Check if the username is already taken
    restDB rDB;
    vector<restEntry> results = rDB.find(name);
    if (results.size() > 0) {
      // Return an error response
      res.set_content("{\"error\":\"Username already taken\"}", "application/json");
    } else {
      // Register the new user
      rDB.addEntry(name,email,password);

      // Return a success response
      res.set_content("{\"message\":\"User registered successfully\"}", "application/json");
    }
});
	
	//code for login!!!!!!!
//code for login!!!!!!!
svr.Get(R"(/chat/login/(.*)/(.*))", [&](const Request& req, Response& res) {
    // Extract the username and password from the request URL
    res.set_header("Access-Control-Allow-Origin","*");
    std::string username = req.matches[1];
    std::string password = req.matches[2];

    // Check if the login credentials are valid
    restDB rDB;
    vector<restEntry> entries = rDB.getUserEntries(username);

    string result;
    // Check if user with this name and password exists
    vector<restEntry> entries = rDB.getUserEntries(username);
    if (entries.empty() || username != entries[0].user || password != entries[0].pass) {
        result = "{\"status\":\"failure\"}";
    } else {
         res.set_content("{\"message\":\"Login successful\"}", "application/json");
            return;
    }

    res.set_content("{\"error\":\"Invalid username or password\"}", "application/json");
});





  cout << "Server listening on port " << port << endl;
  svr.listen("0.0.0.0", port);

}
