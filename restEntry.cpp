#include "restEntry.h"

restEntry::restEntry() {
}

restEntry::restEntry(sql::SQLString ID, sql::SQLString user, sql::SQLString email, sql::SQLString pass) {
    this->ID = ID;
    this->user = user;
    this->email = email;
    this->pass = pass;
}

string restEntry::text() {
	string result = ID + ". ";
	result += user + " ";
	result += email + " ";
	result += pass;
	return result;

}

string restEntry::json() {
	string result = "{\"ID\":\"" + ID + "\",";
	result += "\"user\":\"" + user + "\",";
	result += "\"email\":\"" + email + "\",";
	result += "\"pass\":\"" + pass + "\"}";
	return result;

}
