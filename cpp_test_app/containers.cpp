#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "containers.h"

using namespace std;
using namespace containers;

hash_test::hash_test()
{
    cout << "hash_test constructor" << endl;
    _um["one"] = 1;
    _um["two"] = 2;
    _um["three"] = 3;
    _um["four"] = 4;
}

void hash_test::iterator_test()
{
    cout << "hash_test iterator_test" << endl;
    // define a lambda function takes a pair<string, int> and returns void
    auto print_pair = [](pair<string, int> p) {
        cout << p.first << " => " << p.second << endl;
    };
    cout << ">> for_each" << endl;
    for_each(hash_test::_um.begin(), hash_test::_um.end(), print_pair);
    cout << ">> for auto" << endl;
    for(auto p : _um) {
        cout << p.first << " => " << p.second << endl;
    }
    cout << ">> for iterator" << endl;
    for(auto it = _um.begin(); it != _um.end(); it++) {
        cout << it->first << " => " << it->second << endl;
    }

    cout << ">> find `three` - " << _um.find("three")->second << endl;
}

int hash_test::findOddOccurencesInArray(vector<int> A)
{
    cout << "findOddOccurencesInArray " << endl;
    unordered_map<int, int> um;
    int result = 0;
    for (auto ele : A) {
      cout << " - " << ele;
        um[ele]++;
    }
    cout << endl;
    for (auto it = um.begin(); it != um.end(); it++) {
        if (it->second % 2 != 0) {
            result = it->first;
            break;
        }
    }
    return result;
}

void vector_test::vector_test1() noexcept {
    cout << "vector_test1" << endl;
    vector<int> v = {1, 2, 3, 4, 5};
    for (auto ele : v) {
        cout << ele << endl;
    }
}