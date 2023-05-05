#include <vector>
#include <iostream>
#include <mariadb/conncpp.hpp>
#include "restDB.h"
#include "restEntry.h"



restDB::restDB() {
  	// Instantiate Driver
  	driver = sql::mariadb::get_driver_instance();
  	
 	// Configure Connection
  	// The URL or TCP connection string format is
  	// ``jdbc:mariadb://host:port/database``.
  	sql::SQLString url(db_url);

    // Use a properties map for the other connection options
  	sql::Properties my_properties({{"user", user}, {"password",pass}});
  	// Save properties in object
  	properties = my_properties;

    // Establish Connection
  	std::unique_ptr<sql::Connection> my_conn(driver->connect(db_url, properties));
    
    // Check success
    if (!my_conn) {
   		cerr << "Invalid database connection" << endl;
   		exit (EXIT_FAILURE);
   	}	
   	
   	// Save connection in object
   	conn = std::move(my_conn);
   	
}

vector<restEntry> restDB::find(string search) {

	vector<restEntry> list;
    
    // Make sure the connection is still valid
    if (!conn) {
   		cerr << "Invalid database connection" << endl;
   		exit (EXIT_FAILURE);
   	}	
    // Create a new Statement
	std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
    
    // Execute query
    sql::ResultSet *res = stmnt->executeQuery(
			"SELECT * FROM rest WHERE user like '%"+search+"%' OR "+
    		 + "pass like '%"+search+"%'");
    
    // Loop through and print results
    while (res->next()) {
    	restEntry entry(res->getString("ID"),res->getString("user"),
			res->getString("email"),res->getString("pass"));
	    list.push_back(entry);

    }
    return list;

}

//Get entries for given username
vector<restEntry> restDB::getUserEntries(string username) {
    vector<restEntry> entries;
    
    // Make sure the connection is still valid
    if (!conn) {
        cerr << "Invalid database connection" << endl;
        exit (EXIT_FAILURE);
    }

    // Create a new statement
    std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT * FROM rest WHERE user = ?"));
    stmnt->setString(1, username);

    // Execute query
    sql::ResultSet *res = stmnt->executeQuery();

    // Loop through and add rest entries
    while (res->next()) {
        restEntry entry(res->getString("ID"), res->getString("user"), res->getString("email"), res->getString("pass"));
        entries.push_back(entry);
    }

    return entries;
}


void DB::addEntry(string user,string email,string pass){

	if (!conn) {
   		cerr << "Invalid database connection" << endl;
   		exit (EXIT_FAILURE);
  	}

  	std::auto_ptr<sql::Statement> stmnt(conn->createStatement());

  	stmnt->executeQuery("INSERT INTO rest(user,email,pass) VALUES ('"+user+"','"+email+"','"+pass+"')");
}
