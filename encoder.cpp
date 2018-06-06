#include <iostream>
#include <string.h>

using namespace std;

class encoder
{

};

class decoder
{

};


int main(int argc, char **argv)
{
    cout << "argc = " << argc << "\n";
    //if(argv[1] == "encode")
    
    if(argc > 1)
    {
        if(strcmp("encode", argv[1]) == 0)
            cout << "encoding\n";
        else if(strcmp("decode", argv[1]) == 0)
            cout << "decoding\n";
    }
}
