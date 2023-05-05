#include <mariadb/conncpp.hpp>
#include <string>
using namespace std;

#ifndef CHATENTRY_H
#define CHATENTRY_H

class chatEntry {
public:
     chatEntry();
     chatEntry(sql::SQLString ID, sql::SQLString user, sql::SQLString email, sql::SQLString pass);
     string text();
     string json();
     
  string ID;
  string user;
  string email;
  string pass;;

private:

};

#endif /* CHATENTRY_H */
