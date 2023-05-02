#include "UserManager.h"

UserManager::UserManager() {
}

bool UserManager::registerUser(const std::string &username, const std::string &email, const std::string &password, std::string &error) {
    // Checks if the user already exists
    if (userCredentials.count(username) > 0) {
        error = "Username already exists";
        return false;
    }
	if (userEmails.count(email) > 0) { 
        error = "Email already exists";
        return false;
    }
	 if (password.length() < 6) { 
        error = "Password must be at least 6 characters";
        return false;
    }

    // Saves the user's credentials
    userCredentials[username] = password;
	 userEmails[email] = username; 

    // Registration successful
    return true;
}

bool UserManager::loginUser(const std::string &username, const std::string &password, std::string &error) {
    if (userCredentials.count(username) == 0) {
        error = "Username does not exist.";
        return false;
    }

    if (userCredentials[username] != password) {
        error = "Incorrect password.";
        return false;
    }

    return true;
}

bool UserManager::userExists(const std::string &username) {
    return userCredentials.count(username) > 0;
}

void UserManager::logoutUser(const std::string &username) {
  if (userCredentials.count(username) > 0) {
    userCredentials.erase(username);
  }
}
