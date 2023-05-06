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




  cout << "Server listening on port " << port << endl;
  svr.listen("0.0.0.0", port);

}
