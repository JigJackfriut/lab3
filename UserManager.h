#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <iostream>
#include <map>

class UserManager {
private:
    std::map<std::string, std::string> userCredentials;
	std::map<std::string, std::string> userEmails;
public:
    UserManager();
    bool registerUser(const std::string &username, const std::string &email, const std::string &password, std::string &error);
    bool loginUser(const std::string &username, const std::string &password, std::string &error);
    bool userExists(const std::string &username);
	void logoutUser(const std::string &username);
	
};

#endif // USERMANAGER_H
