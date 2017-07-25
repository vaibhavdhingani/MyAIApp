#include "../inc/learner.h"
#include "../inc/memory.h"
#include "../inc/mySqlWrapper.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

void Learner::respond(string phrase)
{
	int ret = 0;
	memory mem;
	MSG_TYPE msg;


	if(phrase == "bye")
	{
		voice.say("Bye, Have a nice day!");
		exit(0);
	}

	mem.OpenMemory();
	msg = mem.MiningMem(phrase);
	cout << "MiningMem Returns = " << ret << endl;

	if(msg > 0)
	{	
		mem.CallShortMem(phrase, msg);
		voice.say(phrase);
		//voice.say("I did it");
		return;
	}
	else
	{
		string response;
		response = mem.getTableData(phrase);
		if(response.size() > 1)
		{
			voice.say(response);
			mem.CloseMemory();
			return ;
		}

		// Learn new things and store into memory
		voice.say("I don't know what you just said.");
		voice.say("I will like to know more on this.");

		//voice.say(phrase);
		string userResponse;
		cout << "You: ";
		getline(cin, userResponse);
		if(phrase == "bye")
		{
			voice.say("Bye, Have a nice day !!");
			exit(0);
		}
		else
		{
			mem.addMemDataRow(phrase, userResponse);
		}



#if 0
		cout << __func__ << " : " << __LINE__ << endl;
		mem.trainMemory();
		cout << __func__ << " : " << __LINE__ << endl;
		mem.updateMem("hi","Helloooooo");
		cout << __func__ << " : " << __LINE__ << endl;
		mem.deleteMem("How are you?");
		cout << __func__ << " : " << __LINE__ << endl;
		mem.memTransactionTest();
		cout << __func__ << " : " << __LINE__ << endl;
		mem.memAutoIncrementTest();
		cout << __func__ << " : " << __LINE__ << endl;
		mem.printMem();
		cout << __func__ << " : " << __LINE__ << endl;
#endif
		//mem.CloseMemory();
	}
}

void Learner::say(string phrase)
{
	this->voice.say(phrase);
}
