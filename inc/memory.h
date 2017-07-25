
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "mySqlWrapper.h"

using namespace std;

#include "sqlite3.h"
#define DB "/home/vaibhavdhingani/test.db"

enum MSG_TYPE
{
	NOT_FOUND = -1,
	GOOGLE = 1,
	MAPS,
	YOUTUBE,
	START_MUSIC,
	PAUSE_MUSIC,
	STOP_MUSIC,

	START_VIDEO,
	PAUSE_VIDEO,
	STOP_VIDEO,
	
	VOLUME_UP,
	VOLUME_DOWN,
	VOLUME_MUTE,
	VOLUME_UNMUTE,
	TAKE_SNAP,
	STREAM_VIDEO,
	STREAM_CAMERA,
	DATE,
	TIME,
	WEATHER,

	
	COMMAND,
	GENERIC,
};

class memory
{
	private:
		string MemQue;
		string MemAns;

	public:
		memory()
		{
			MemQue = ""; 
			MemAns = ""; 
		}
		memory(string que, string ans)
		{
			MemQue = que;
			MemAns = ans;
		}

		void setMemQue(string que) { MemQue = que; }
		void setMemAns(string ans) { MemAns = ans; }

		string getMemQue() { return MemQue; }
		string getMemAns() { return MemAns; }

		bool OpenMemory ();
		void CloseMemory ();

		MSG_TYPE MiningMem(string data);
		int addMemDataRow(string memQue, string memAns);
		string getTableData(string que);
		void CallShortMem(string phrase, MSG_TYPE msgType);

		memory trainMemory();
		int updateMem(string que, string ans);
		int deleteMem(string que);
		int memAutoIncrementTest();
		int printMem();
		int memTransactionTest();
		int memTest();
};
