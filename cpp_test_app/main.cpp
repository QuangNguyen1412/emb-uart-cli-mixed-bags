#include <iostream>
#include <cstdio>
#include <vector>
#include <list>
#include <string>
#include "main.h"

using namespace std;

class Town
{
public:
    Town(string townName, vector<string> companies) {
        _companies = companies;
        _townName = townName;
    }

    Town() {
        _townName = "";
    }

    vector<string> getCompanies() {
        return _companies;
    }
    
    string getTownName() {
        return _townName;
    }
private:
    vector<string> _companies;
    string _townName;
};

class People
{
public:
    People(string name, Town town) {
        _town = town;
        _name = name;
    }
    
    Town getTown() {
        return _town;
    }
    
    string getName() {
        return _name;
    }
private:
    Town _town;
    string _name;
};


Town springField = Town("SpringField", {"Ivanti", "Abc"});
Town sandy = Town("Sandy", {"payday loan", "carwash"});
Town saltLake = Town("Sale Lake", {"Goal Zero", "Teva"});
list<People> globalPeople = {{"Quang", springField},
                              {"someone1", sandy},
                              {"Na", saltLake}};

list<People> Find(string townName, string companyName) {
    list<People> ret;
    for(auto person : globalPeople) {
      if (townName == person.getTown().getTownName()) {
          for (auto company: person.getTown().getCompanies()) {
              if (company == companyName) {
                  ret.push_back(person);
                  break;
              }
          }
      }
    }
    return ret;
}

#define TEMP 3
int main() {
  list<People> ret = Find("SpringField", "Ivanti");
  for(auto person : ret) {
      cout << person.getName() << endl;
  }

  ret.clear();
  ret = Find("Sandy", "carwash");
  for(auto person : ret) {
      cout << person.getName() << endl;
  }

  // dynamically allocated 2 dimensional array, and deletion
  char **data = new char* [10];
  for (int i = 0; i < 10; i++) {
      data[i] = new char[10];
  }
  strcpy(data[0], "cat");
  strcpy(data[1], "dog");
  for (int i = 0; i < 2; i++) {
      cout << data[i] << endl;
  }

  // Delete the dynamically allocated memory
  for (int i = 0; i < 10; i++) {
      delete[] data[i];
  }
  delete[] data;

  // test class
  SomeTestClass test;
  test.ProcessString("some test class data");

  // test macro
  int i = TEMP + 1;
  cout << "i: " << i << endl;
}