#include <iostream>
#include <stdlib.h>
#include "../inc/learner.h"

using namespace std;

int main()
{
    Learner AI;
    
    for(;;)
    {
        cout << "\nYou: ";
        string phrase;
        getline (cin, phrase);
    
        cout << "DeViz: ";
        AI.respond(phrase);
    }
}
