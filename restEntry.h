#include <mariadb/conncpp.hpp>
#include <string>
using namespace std;

#ifndef RESTENTRY_H
#define RESTENTRY_H

class restEntry {
public:
     restEntry();
     restEntry(sql::SQLString ID, sql::SQLString user, sql::SQLString email, sql::SQLString pass);
     string text();
     string json();
     
  string ID;
  string user;
  string email;
  string pass;;

private:

};

#endif /* CHATENTRY_H */
