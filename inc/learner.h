#ifndef _LEARNER_H
#define _LEARNER_H

#include <iostream>
#include <fstream>
#include "voice.h"

using namespace std;

class Learner
{
    public:
        void respond(string phrase);
        void say(string phrase);

        Voice voice;
};

#endif
