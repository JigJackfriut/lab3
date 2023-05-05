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

#include "chatDB.h"
#include "chatEntry.h"

using namespace httplib;
using namespace std;

const int port = 5005;

//Username-Token Map
map<string,string> userTokenMap;

//function to generate random token
string generateToken(int length) {
    string result(length, ' ');
    const string allowedCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; ++i) {
        int randomIndex = rand() % allowedCharacters.size();
        result[i] = allowedCharacters[randomIndex];
    }
    return result;
}


void addMessage(string username, string message, map<string,vector<string>> &messageMap) {
	/* iterate through users adding message to each */
	string jsonMessage = "{\"user\":\""+username+"\",\"message\":\""+message+"\"}";
	for (auto userMessagePair : messageMap) {
	username = userMessagePair.first;
	messageMap[username].push_back(jsonMessage);
	}
}


void addUser(string username, string password, string email, map<string,string> &userMap) {
    string jsonMessage = "{\"user\":\""+username+"\",\"pass\":\""+password+"\",\"email\":\""+email+"\"}";
    userMap[username] = jsonMessage;
	string user=username, pass=password;
	
	//added so that the database table is updated whenever someone successfully registers
	/*chatDB cDB;
	cDB.addEntry(user,email,pass);*/
}



string getMessagesJSON(string username, map<string,vector<string>> &messageMap) {
	/* retrieve json list of messages for this user */
	bool first = true;
	string result = "{\"messages\":[";
	for (string message : messageMap[username]) {
	if (not first) result += ",";
	result += message;
	first = false;
	}
	result += "]}";
	messageMap[username].clear();
	return result;
}


int main(void) {
 chatDB cDB;
 
 Server svr;

 int nextUser=0;
 map<string,vector<string>> messageMap;
 map<string,string> userMap;
 map<string,string> userEmail;

	
 /* "/" just returnsAPI name */
 svr.Get("/", [](const Request & /*req*/, Response &res) {
 res.set_header("Access-Control-Allow-Origin","*");
 res.set_content("Chat API", "text/plain");
 });


 //microservice for registration with username, email and password
 svr.Get(R"(/chat/register/(.*)/(.*)/(.*))", [&](const Request& req, Response& res) {
	res.set_header("Access-Control-Allow-Origin","*");
 	string username = req.matches[1];
	string email = req.matches[2];
	string password = req.matches[3];
	 
 	string result;
 	vector<string> empty;

	 vector<chatEntry> entries = cDB.getUserEntries(username);
bool usernameExists = false;
for (const auto& entry : entries) {
    if (entry.user == username) {
        usernameExists = true;
        break;
    }
}
	 
 if (usernameExists or messageMap.count(username) or messageMap.count(email) or password.length() < 7){
 result = "{\"status\":\"registrationfailure\"}";
 } else {
 messageMap[username]= empty;
	userEmail[username] = email;
	chatDB cDB;
	string user=username; string pass=password;
	cDB.addEntry(user,email,pass);
	addUser(username , password, email , userMap);
 result = "{\"status\":\"success\",\"user\":\"" + username + "\",\"email\":\"" + email + "\",\"pass\":\"" + password + "\"}";
 }
	


/*vector<chatEntry> entries = cDB.getUserEntries(username);
bool usernameExists = false;
for (const auto& entry : entries) {
    if (entry.user == username) {
        usernameExists = true;
        break;
    }
}

if (usernameExists) {
    result = "{\"status\":\"registrationfailure\"}";
} else {
    result = "{\"status\":\"success\",\"user\":\"" + username + "\",\"email\":\"" + email + "\",\"pass\":\"" + password + "\"}";
}*/
	 
 res.set_content(result, "text/json");
 });
 
 
//edited for joining with username and password
 /*svr.Get(R"(/chat/join/(.*)/(.*))", [&](const Request& req, Response& res) {
 res.set_header("Access-Control-Allow-Origin","*");
 	string username = req.matches[1];
	string password = req.matches[2];
	//string userDetails= "{\"user\":\""+username+"\",\"pass\":\""+password+"\"}";
	//cout<<userDetails;
	vector<chatEntry> entries = cDB.getUserEntries(username);       
	    //cout << "User: " << entries[0].user << endl;
	    //cout << "Password: " << entries[0].pass << endl;

 string result;
 // Check if user with this name and password exists
 if (username==entries[0].user && password==entries[0].pass){
	 
	 string token=generateToken(10); //generate a token of length 10 and add it to the usertoken map
	 userTokenMap[username] = token;
	 
  result = "{\"status\":\"success\",\"user\":\"" + username + "\",\"token\":\"" + token + "\"}"; //modify result json to include token
	cout << username << " joins. Token: " << token << endl;
	addUser(username , password, entries[0].email , userMap);
	 
 } else {
 result = "{\"status\":\"failure\"}";
 }
 res.set_content(result, "text/json");
 });*/
	
svr.Get(R"(/chat/join/(.*)/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    string username = req.matches[1];
    string password = req.matches[2];

    string result;
    // Check if user with this name and password exists
    vector<chatEntry> entries = cDB.getUserEntries(username);
    if (entries.empty() || username != entries[0].user || password != entries[0].pass) {
        result = "{\"status\":\"failure\"}";
    } else {
        string token = generateToken(10);
        userTokenMap[username] = token;
        result = "{\"status\":\"success\",\"user\":\"" + username + "\",\"token\":\"" + token + "\"}";
        cout << username << " joins. Token: " << token << endl;
	   addUser(username , password, entries[0].email , userMap);
    }

    res.set_content(result, "text/json");
});


 

 svr.Get(R"(/chat/send/(.*)/(.*))", [&](const Request& req, Response& res) {
 res.set_header("Access-Control-Allow-Origin","*");
	string token = req.matches[1];
	string message = req.matches[2];
	string result;
	 
	 bool userExists = false;
    string username;
    for (auto const& [key, val] : userTokenMap) {
        if (val == token) {
            userExists = true;
            username = key;
            break;
        }
    }
	 
 //if (!messageMap.count(username) or !userExists) {
if (!userExists) {
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
    string resultJSON = getMessagesJSON(username, messageMap);
    res.set_content(resultJSON, "text/json");
});

 
//microservice for setting up user list in json	
svr.Get(R"(/chat/users)", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "text/json");
    
    string result = "{ \"users\": [";
    for (const auto& [username, userdata] : userMap) {
        result += "\"" + username + "\", ";
    }
    if (!userMap.empty()) {
        result.erase(result.length() - 2);
    }
    result += "] }";
    
    res.set_content(result, "text/json");
});

//microservice for removing users that leave from the usermap
svr.Get(R"(/chat/users/remove/(.*))", [&](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin","*");
    string username = req.matches[1];
    userMap.erase(username);

//remove token when logout
    userTokenMap.erase(username); 
});

 
 cout << "Server listening on port " << port << endl;
 svr.listen("0.0.0.0", port);

}
