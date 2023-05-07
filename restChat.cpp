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





int main(void) {
  Server svr;
  restDB rDB;

	
  /* "/" just returnsAPI name */
  svr.Get("/", [](const Request & /*req*/, Response &res) {
    res.set_header("Access-Control-Allow-Origin","*");
    res.set_content("Chat API", "text/plain");
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
svr.Get(R"(/chat/login/(.*)/(.*))", [&](const Request& req, Response& res) {
    // Extract the username and password from the request URL
    res.set_header("Access-Control-Allow-Origin","*");
    std::string username = req.matches[1];
    std::string password = req.matches[2];

    // Check if the login credentials are valid
    restDB rDB;
	vector<restEntry> entries = getUserEntries(username);

    // Check if any of the entries match the given password
    for (auto entry : entries) {
        if (entry.getPassword() == password) {
            // Password matches, return true
                    res.set_content("{\"message\":\"Login successful\"}", "application/json");

        }
    }
        res.set_content("{\"error\":\"Invalid username or password\"}", "application/json");
    
   
});





  cout << "Server listening on port " << port << endl;
  svr.listen("0.0.0.0", port);

}
