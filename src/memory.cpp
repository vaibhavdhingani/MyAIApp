#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "../inc/memory.h"
#include "../inc/mySqlWrapper.h"
using namespace std;

bool isMemoryOpen = false;
SqlDB db;

bool memory::OpenMemory ()
{
	string SqlQuery;

	db.open(DB);
	if(false == db.tableExists("LongTermMemory"))
	{
		cout << endl << "Creating LongTermMemory table" << endl;
		db.execDML("create table LongTermMemory(MemQuestion char(20), MemAnswer char(20));");
	} else {
		cout << "LongTermMemory table exists." << endl;
		return true;
	}

	if(false == db.tableExists("ShortTermMemory"))
	{
		cout << endl << "Creating ShortTermMemory table" << endl;
		db.execDML("create table ShortTermMemory(MemQuestion char(20), MemAnswer char(20));");
	} else {
		cout << "ShortTermMemory table exists." << endl;
		return true;
	}
	return false;
}


void memory::CloseMemory ()
{
	db.close();
}


MSG_TYPE memory::MiningMem(string data)
{
	int i, fld;
	int nRows;

	////////////////////////////////////////////////////////////////////////////////
	// Fetch table at once, and also show how to use CppSQLiteTable::setRow() method
	////////////////////////////////////////////////////////////////////////////////
	cout << endl << "MiningMem() test" << endl;
	if(data.compare(0, 13, "search google") == 0)
		return GOOGLE;
	if(data.compare(0, 13, "search videos") == 0)
		return YOUTUBE;

	if(data.compare(0, 11, "search maps") == 0)
		return MAPS;

	if(data.compare(0, 8, "where is") == 0)
		return MAPS;

	if(data.compare(0, 11, "start music") == 0)
		return START_MUSIC;

	if(data.compare(0, 10, "stop music") == 0)
		return STOP_MUSIC;
	
	if(data.compare(0, 11, "pause music") == 0)
		return PAUSE_MUSIC;

	if(data.compare(0, 11, "start video") == 0)
		return START_VIDEO;

	if(data.compare(0, 10, "stop video") == 0)
		return STOP_VIDEO;
	
	if(data.compare(0, 11, "pause video") == 0)
		return PAUSE_VIDEO;


	if(data.compare(0, 9, "volume up") == 0)
		return VOLUME_UP;

	if(data.compare(0, 11, "volume down") == 0)
		return VOLUME_DOWN;
	if(data.compare(0, 11, "volume mute") == 0)
		return VOLUME_MUTE;

	if(data.compare(0, 13, "volume unmute") == 0)
		return VOLUME_UNMUTE;

	if(data.compare(0, 9, "take snap") == 0)
		return TAKE_SNAP;

	if(data.compare(0, 4, "date") == 0)
		return DATE;
	if(data.compare(0, 4, "time") == 0)
		return TIME;


	if(data.compare(0, 12, "stream video") == 0)
		return STREAM_VIDEO;


	if(data.compare(0, 13, "stream camera") == 0)
		return STREAM_CAMERA;


	if(data.compare(0, 10, "weather of") == 0)
		return WEATHER;


	SqlTable t = db.getTable("select * from ShortTermMemory order by 1;");

	for (int row = 0; row < t.numRows(); row++)
	{
		t.setRow(row);
		for (int fld = 0; fld < t.numFields(); fld++)
		{
			if (!t.fieldIsNull(fld))
			{
				if(data == t.fieldValue(fld))
					return COMMAND;
			}
		}
	}
	return NOT_FOUND;
}


void memory::CallShortMem(string phrase, MSG_TYPE msgType)
{
	string command;
	string str;

	switch(msgType)
	{
		case GOOGLE:
			str = phrase.substr(13);
			command = "xdg-open \"http://www.google.com/search?q=" + str + "\"";
			break;
		case YOUTUBE:
			str = phrase.substr(13);
			command = "xdg-open \"https://www.youtube.com/results?search_query=" + str + "\"";
			break;
		case MAPS:
			str = phrase.substr(11);
			command = "xdg-open \"https://www.google.com/maps?q=" + str + "\"";
			break;
		case START_MUSIC:
			str = phrase.substr(11);
			if(str.empty() == true)
				command = "rhythmbox-client --play";
			else
				command = "rhythmbox " + str + "&";
			break;
		case STOP_MUSIC:
			command = "rhythmbox-client --quit";
			break;
		case PAUSE_MUSIC:
			command = "rhythmbox-client --pause";
			break;

		case START_VIDEO:
			str = phrase.substr(11);
			if(str.empty() == true)
				command = "totem ~/Videos/ &";
			else
				command = "totem " + str + "&";
			break;
		case STOP_VIDEO:
			command = "totem --quit";
			break;
		case PAUSE_VIDEO:
			command = "totem --pause";
			break;

		case WEATHER:
			str = phrase.substr(10);
			command = "curl wttr.in/" + str;
			break;


		case VOLUME_UP:
			command = "amixer -D pulse sset Master 20%+";
			break;
		case VOLUME_DOWN:
			command = "amixer -D pulse sset Master 20%-";
			break;
		case VOLUME_MUTE:
			command = "amixer -D pulse sset Master mute";
			break;
		case VOLUME_UNMUTE:
			command = "amixer -D pulse sset Master unmute";
			break;

		case TAKE_SNAP:
			command = "streamer -f jpeg -s 1920x1080 -o ~/Pictures/snap.jpeg";
			system("notify-send -i ~/Pictures/snap.jpeg \"Captured !\"");
			break;

		case DATE:
			command = "notify-send -i ~/Pictures/icon6.svg  \"Date\" \"`date \"+%F\"`\"";
			break;
	
		case TIME:
			command = "notify-send -i ~/Pictures/icon6.svg  \"Time\" \"`date \"+%r\"`\"";
			break;
		
		case STREAM_VIDEO:
			command = "vlc -vvv ~/Desktop/source.mp4 --sout '#transcode{vcodec=mp2v,vb=800,acodec=none}:rtp{dst=10.245.227.102,port=5004,mux=ts}'";
			break;

		case STREAM_CAMERA:
			command = "gst-launch -v v4l2src device=/dev/video0 ! video/x-raw-yuv,framerate=30/1,width=1280,height=720 ! ffenc_mpeg4 ! rtpmp4vpay send-config=true ! udpsink host=10.245.227.102 port=5000 &";
			break;

		case NOT_FOUND:
			break;
		case COMMAND:
			break;
		default:
			;/* Do Nothing */
	}

	//string command = "voice-commands -try \"" + phrase + "\"";
	const char* charCommand = command.c_str();
	cout << phrase << endl;
	system(charCommand);
	return ;
}

int memory::addMemDataRow(string memQue, string memAns)
{
	// get the student data from user 
	std::stringstream strm;
	strm << "insert into memory(MemQuestion,MemAnswer) values('"
		<< memQue << "','" << memAns << "')";

	string s = strm.str();
	char *str = &s[0];
	cout << str << endl;

	int nRows = db.execDML(str);
	cout << nRows << " rows inserted" << endl;

	return 0;
}

string memory::getTableData(string que)
{
	sqlite3_stmt *statement;
	int fld = 0;
	string chr = "select * from memory where MemQuestion= '" + que + "';";
	char *query = strdup(chr.c_str());
	string ret;

	cout << query << endl;

	SqlQuery q = db.execQuery(query);

	while (!q.eof())
	{
		cout << q.fieldValue(0) << "|";
		cout << q.fieldValue(1) << "|" << endl;
		ret = q.fieldValue(1);
		q.nextRow();
	}
	return ret;
}


memory memory::trainMemory()
{
	memory mem;

	string que;
	string ans;

	cout << " Enter Question : " ;
	getline(cin, que);
	cout << " Enter Answer : " ;
	getline(cin, ans);

	mem.setMemQue(que);
	mem.setMemAns(ans);

	mem.addMemDataRow(que, ans);
	return mem;
}


int memory::updateMem(string que, string ans)
{
	int nRows = 0;

	string chr = "update memory set MemAnswer= '" + ans + "' where MemQuestion= '" + que + "';";
	char *query = strdup(chr.c_str());

	nRows = db.execDML(query);
	cout << nRows << " rows updated" << endl;

	return nRows;
}

int memory::deleteMem(string que)
{
	int nRows = 0;

	string chr = "delete from memory where MemQuestion= '" + que + "';";
	char *query = strdup(chr.c_str());

	nRows = db.execDML(query);
	cout << nRows << " rows updated" << endl;

	return nRows;

}

int memory::memTransactionTest()
{
	int i =0;
	time_t tmStart, tmEnd;
	////////////////////////////////////////////////////////////////////////////////
	// Transaction Demo
	// The transaction could just as easily have been rolled back
	////////////////////////////////////////////////////////////////////////////////
	int nRowsToCreate(50);
	cout << endl << "Transaction test, creating " << nRowsToCreate;
	cout << " rows please wait..." << endl;
	tmStart = time(0);
	db.execDML("begin transaction;");

	for (i = 0; i < nRowsToCreate; i++)
	{
		char buf[128];
		sprintf(buf, "insert into memory values ('Que%06d', 'Ans%06d');", i, i); 
		db.execDML(buf);
	}

	db.execDML("commit transaction;");
	tmEnd = time(0);

	////////////////////////////////////////////////////////////////////////////////
	// Demonstrate CppSQLiteDB::execScalar()
	////////////////////////////////////////////////////////////////////////////////
	cout << db.execScalar("select count(*) from memory;") << " rows in memory table in ";
	cout << tmEnd-tmStart << " seconds (that was fast!)" << endl;

	return 0;
}



int memory::memAutoIncrementTest()
{
	int i, fld;
	int nRows;
	////////////////////////////////////////////////////////////////////////////////
	// Re-create emp table with auto-increment field
	////////////////////////////////////////////////////////////////////////////////
	cout << endl << "Auto increment test" << endl;
	//db.execDML("create table memory(MemQuestion char(20), MemAnswer char(20));");
	cout << nRows << " rows deleted" << endl;

	for (i = 0; i < 5; i++)
	{
		char buf[128];
		sprintf(buf, "insert into memory values ('Que%06d', 'Ans%06d');", i, i); 
		db.execDML(buf);
		cout << " primary key: " << (int)db.lastRowId() << endl;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Query data and also show results of inserts into auto-increment field
	////////////////////////////////////////////////////////////////////////////////
	cout << endl << "Select statement test" << endl;
	SqlQuery q = db.execQuery("select * from memory order by 1;");

	for (fld = 0; fld < q.numFields(); fld++)
	{
		cout << q.fieldName(fld) << "(" << q.fieldDeclType(fld) << ")|";
	}
	cout << endl;

	while (!q.eof())
	{
		cout << q.fieldValue(0) << "|";
		cout << q.fieldValue(1) << "|" << endl;
		q.nextRow();
	}

	return 0;
}



int memory::printMem()
{
	int i, fld;
	int nRows;

	////////////////////////////////////////////////////////////////////////////////
	// Fetch table at once, and also show how to use CppSQLiteTable::setRow() method
	////////////////////////////////////////////////////////////////////////////////
	cout << endl << "getTable() test" << endl;
	SqlTable t = db.getTable("select * from memory order by 1;");

	for (fld = 0; fld < t.numFields(); fld++)
	{
		cout << t.fieldName(fld) << "|";
	}
	cout << endl;
	for (int row = 0; row < t.numRows(); row++)
	{
		t.setRow(row);
		for (int fld = 0; fld < t.numFields(); fld++)
		{
			if (!t.fieldIsNull(fld))
				cout << t.fieldValue(fld) << "|";
			else
				cout << "NULL" << "|";
		}
		cout << endl;
	}

}

int memory::memTest()
{
	int i, fld;
	int nRows;

	int nRowsToCreate(50);
	time_t tmStart, tmEnd;

	////////////////////////////////////////////////////////////////////////////////
	// Pre-compiled Statements Demo
	////////////////////////////////////////////////////////////////////////////////
	cout << endl << "Transaction test, creating " << nRowsToCreate;
	cout << " rows please wait..." << endl;
	db.execDML("drop table emp;");
	db.execDML("create table emp(empno int, empname char(20));");
	tmStart = time(0);
	db.execDML("begin transaction;");

	SqlStatement stmt = db.compileStatement("insert into emp values (?, ?);");
	for (i = 0; i < nRowsToCreate; i++)
	{
		char buf[16];
		sprintf(buf, "EmpName%06d", i); 
		stmt.bind(1, i); 
		stmt.bind(2, buf);
		stmt.execDML();
		stmt.reset();
	}

	db.execDML("commit transaction;");
	tmEnd = time(0);

	cout << db.execScalar("select count(*) from emp;") << " rows in emp table in ";
	cout << tmEnd-tmStart << " seconds (that was even faster!)" << endl;
	cout << endl << "End of tests" << endl;

}
