#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "httplib.h"
#include "restDB.h"
#include "restEntry.h"

int main() {
  restDB cdb;
  
  vector<restEntry> results;

  results = cdb.find("");

  for (int i = 0; i<results.size(); i++) {
    cout << results[i].text() << endl;
  }
}
