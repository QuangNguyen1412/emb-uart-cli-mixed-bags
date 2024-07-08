#include <unordered_map>
#include <iostream>
#include <string>

namespace containers
{
class hash_test
{
private:
  std::unordered_map<std::string, int> _um;
public:
    hash_test();
    // {
        // std::cout << "hash_test constructor" << endl;
        // _um["one"] = 1;
        // _um["two"] = 2;
        // cout << "_um bucket " << _um.bucket_count() << endl;
    // }
    void iterator_test();
    int findOddOccurencesInArray(std::vector<int> A);
    ~hash_test()
    {
        std::cout << "hash_test destructor" << std::endl;
    }
};

class vector_test
{
public:
  static void vector_test1() noexcept;
};
}
