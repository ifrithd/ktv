#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

bool is_admin;// 标志是否是管理员
string role[2] = {"USER", "ADMIN"};
bool running;// 标志程序是否继续循环
map<string, string> songs;// 字典，曲名为键，歌手为值
vector<string> singers; // 储存歌手名字

void admin(bool b)
{
	if (is_admin == b)
	{
		cout << "You are already " <<  role[is_admin];
		cout << ", so nothing has changed." << endl;
	}
	else
	{
		is_admin = b;
		cout << "Successfully change to " << role[is_admin] << endl;
	}
}

void invalid(string s)
{
	cout << "Invalid command " << '"' << s << "\"!" << endl;
	cout << "Please try \"help\" for help" << endl;
}

void help(string cmd)
{
	/* 尝试从相应文件中读取帮助信息 */
	string route;
	route = "help\\" + cmd + ".txt";
	ifstream fin(route);
	
	if (!fin)
	{
		invalid(cmd);
		return;
	}
	
	for (string s;getline(fin, s);)
		cout << s << endl;
}

void play(string song)
{
	/* 查询曲库中是否有这首歌 */
	auto iter = songs.find(song);
	if (iter == songs.end())
	{
		cout << "Can not find song " << "\"" << song << "\"." << endl;
		cout << "Please try \"song\" for songs info." << endl;
		return;
	}
	
	/* 寻找歌词文件 */
	string route;
	route = "lyrics\\" + song + ".txt";
	ifstream fin(route);
	if (!fin)
	{
		cout << "Can not find lyrics of the song." << endl;
		cout << "Please contact the administor." << endl;
		return;
	}
	
	cout << "Song name: " << song << endl;
	cout << "Singer: " << songs[song] << endl << endl;
	for (string s;getline(fin, s);)
		cout << s << endl;
}

void song(string str)
{
	/* 打印所有歌曲信息 */
	if (str.length() == 0)
	{
		int cnt = 0;
		for (auto iter=songs.begin();iter!=songs.end();iter++)
		{
			cnt++;
			cout << setw(3) << cnt << ": ";
			cout << left << setw(20) << iter->first << right;
			cout << "\tSinger: " << iter->second << endl;
		}
		cout << endl << cnt << " songs found." << endl;
		return;
	}
	
	/* 打印单曲信息 */
	auto iter = songs.find(str);
	if (iter == songs.end())
		cout << "Can not find song \"" << str << "\"." << endl;
	else
	{
		cout << "Song: " << iter->first;
		cout << "\tSinger: " << iter->second << endl;
	} 
}

void singer(string str)
{
	/* 打印所有歌手姓名 */
	if (str.length() == 0)
	{
		for (int i=0;i<singers.size();i++)
			cout <<setw(3) << i+1 << ": " << singers[i] << endl;
		cout << endl << singers.size() << " singers found." << endl;
		return;
	}
	
	/* 打印歌手所有歌曲 */
	cout << "Searching songs of " << str << "..." << endl;
	int cnt = 0;
	for (auto iter=songs.begin();iter!=songs.end();iter++)
		if (iter->second == str)
		{
			cnt++;
			cout << setw(3) << cnt << ": ";
			cout << iter->first << endl;
		}
	cout << endl << cnt << " songs found." << endl;
}

int check_admin()
{
	if (is_admin)
		return 1;
	cout << "Permission denied. Try \"admin\"";
	cout << "to get administor privilege." << endl;
	return 0;
}

void add()
{
	if (!check_admin()) return;
	
	cout << "You are going to add a new song.(y/n)";
	char c = cin.get();
	cin.get();
	if (c != 'y') 
	{
		cout << "canceled." << endl;
		return;
	}
	
	
	string songname, singername;
	cout << "Song name: ";
	getline(cin, songname);
	trim(songname);
	cout << "Singer name: ";
	getline(cin, singername);
	trim(singername);
	
	songs[songname] = singername;
	
	string route;
	route = "lyrics\\" + songname + ".txt"; 
	ofstream fout(route);
	if (!fout) {cout << "Open " << songname << ".txt failed" << endl;return;}
	cout << "Input lyrics, type @ to end: " << endl;
	while (c = cin.get())
		if (c == '@') break;
		else fout << c;// 将歌词写入文件
	cin.get();
	
	cout << "Successfully add the new song \"" << songname << "\"." << endl;
}

void del(string str)
{
	if (!check_admin()) return;
	
	auto iter = songs.find(str);
	if (iter == songs.end())
	{
		cout << "Can not find song " << "\"" << str << "\"." << endl;
		cout << "Please try \"song\" for songs info." << endl;
		return;
	}
	
	cout << "You are gonging to delete song\"" << str << "\". ";
	cout << "Are you sure?(y/n)";
	char c = cin.get();
	cin.get();
	if (c != 'y')
	{
		cout << "canceled." << endl;
		return;
	}
	
	songs.erase(iter);
	string route("lyrics\\" + str + ".txt");
	remove(route.c_str());
	
	cout << "Successfully remove the song \"" << str << "\"." << endl;
}

/* 解析并执行命令 */
void execute(string cmd)
{
	if (cmd.length() == 0) return;// 处理空命令
	
	/* 将命令用空格进行划分 */
	trim(cmd);
	vector<string> parse;
	split(parse, cmd, is_any_of(" "), token_compress_on);
	
	if (parse[0] == "exit")
		running = false;
	
	else if (parse[0] == "help")
	{
		if (parse.size() == 1)
			help(string("all"));
		else if (parse.size() == 2)
			help(parse[1]);
	}
	
	else if (parse[0] == "admin")
	{
		if (parse.size() == 1)
			admin(true);
		else if (parse.size() == 2 and parse[1] == "-q")
			admin(false);
		else
			invalid(cmd);
	}
	
	else if (parse[0] == "play")
	{
		if (parse.size() == 1)
		{
			cout << "Please input a song!" << endl;
			return;
		}
		parse.erase(parse.begin());
		play(join(parse, " "));// 将分割的歌名重新合并
	}
	
	else if (parse[0] == "song")
	{
		if (parse.size() == 1)
			song(string(""));
		else
		{
			parse.erase(parse.begin());
			song(join(parse, " "));
		}
	}
	
	else if (parse[0] == "singer")
	{
		if (parse.size() == 1)
			singer(string(""));
		else
		{
			parse.erase(parse.begin());
			singer(join(parse, " "));
		}
	}
	
	else if (parse[0] == "add")
		add();
	
	else if (parse[0] == "del")
	{
		if (parse.size() == 1)
		{
			cout << "Input a song!" << endl;
			return;
		}
		parse.erase(parse.begin());
		del(join(parse, " "));
	}
	
	else 
		invalid(cmd);
}

void init()
{
	is_admin = false;
	running = true;
	
	/* 读入歌曲信息 */
	ifstream fin("songs.txt");
	vector<string> v;
	string s;
	while (getline(fin, s))
	{
		trim(s);
		split(v, s, is_any_of("@"));
		songs[v[0]] = v[1];
		if (find(singers.begin(), singers.end(),v[1]) == singers.end()) // 增加歌手
			singers.push_back(v[1]);
	}
}

/* 将songs的内容保存至文件 */
void save()
{
	ofstream fout("songs.txt");
	for (auto iter=songs.begin();iter!=songs.end();iter++)
		fout << iter->first << '@' << iter->second << endl;
	cout << "Successfully save songs data. Goodbye!" << endl;
}

int main()
{
	init();
	
	string command;
	while (running)
	{
		cout << endl << role[is_admin] << "> ";
		getline(cin, command);
		execute(command);
	}
	
	save();
	
	return 0;
}