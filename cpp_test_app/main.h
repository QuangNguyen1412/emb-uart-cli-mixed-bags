#include <cstring>

class SomeTestClass
{
private:
    char _mydata[32];
public:
    void ProcessString(const char* input) const
    {
       strcpy(const_cast<char *>(_mydata), input); // without this const_cast, the compiler will complain
       std::cout << "Data: " << _mydata << std::endl;
    }
};
