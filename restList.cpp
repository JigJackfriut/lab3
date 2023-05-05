#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "httplib.h"
#include "chatDB.h"
#include "chatEntry.h"

int main() {
  chatDB cdb;
  
  vector<chatEntry> results;

  results = cdb.find("");

  for (int i = 0; i<results.size(); i++) {
    cout << results[i].text() << endl;
  }
}
