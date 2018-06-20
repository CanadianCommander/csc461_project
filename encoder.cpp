#include <iostream>
#include <string.h>
#include <

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
    
    cout << "argc = " << argc << endl;
    
    if(argc < 3)
    {
        cout << "Please enter the correct number of arguments\n";
        return -1;
    }


    //reduntant if
    if(argc > 1)
    {
        if(strcmp("encode", argv[1]) == 0)
            cout << "encoding\n";
        else if(strcmp("decode", argv[1]) == 0)
            cout << "decoding\n";
    }

    //specify filename
    
}
