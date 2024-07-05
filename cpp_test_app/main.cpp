#include <iostream>
#include <cstdio>
#include <vector>
#include <list>
#include <string>

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

int main() {
  cout << "Hello" << endl;
  list<People> ret = Find("SpringField", "Ivanti");
  for(auto person : ret) {
      cout << person.getName() << endl;
  }

  ret.clear();
  ret = Find("Sandy", "carwash");
  for(auto person : ret) {
      cout << person.getName() << endl;
  }
  return 0;
}