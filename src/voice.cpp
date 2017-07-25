#include "../inc/voice.h"
#include <iostream>
#include <stdlib.h>
//#include <windows.h>

using namespace std;

void Voice::say(string phrase)
{
    string command = "espeak \"" + phrase + "\"";
    const char* charCommand = command.c_str();
    cout << phrase << endl;
    system(charCommand);

    string display = "notify-send -i ~/Pictures/icon6.svg 'DeViz' '" + phrase + "'";
    const char* charDisplay = display.c_str();
    system(charDisplay);
}
