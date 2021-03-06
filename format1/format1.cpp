// format1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <graphics.h>
#include <conio.h>
#include<vector>
#include<windows.h>
#include<string>
#include<math.h>
#include<time.h>
#include<set>
#include<map>
#include<iostream>
#include <fstream> 

using namespace std;

//函数声明
void stringToTCHAR(string str, TCHAR s[], int len);

//全局变量设置
const double pi = acos(-1.0);

//圆的半径
int r = 20;

int winh = 618;
int winw = 1000;

int minh = winh / 2;

//点的个数
int n=5;

// 定义鼠标消息
MOUSEMSG m;		

string Path = "./G.txt";

//字符集合 
set<char> E;
//状态集合 
set<string> Q;
//开始符号
string S;

string buff;

//终止符号集合 
set<string> T;
//句子，用来判别的句子
string L = "000111";

int cnt = 0;

class State
{
public:
	//开始状态 
	string state;
	//通过的字符，和到达的目的状态集合 
	map<char, string> toQ;
	//储存状态点在图中的位置和半径大小
	int x, y, r;
	State* readAchar();

	State();
};

State::State()
{
	state = "";
	toQ.clear();
	x = y = r = 0;
}

bool operator < (const State a, const State b)
{
	return a.state < b.state;
}

map<string, State> F;
map<string, State>::iterator itOfSelectedState;

//readAchar函数是识别句子的函数，但现在需要图像话显示所以需要加以改造。使其和画图结合起来展示句子的识别过程。
/*
具体实现是使每个状态节点在调用readAchar时先检测当前时间等待是否完成，该时间等待用来演示一个状态到另一个状态的变化。
当演示完成后，readAchar的等待完成，改变全局的变量，使全局知道，我们当前演示的点和边现在又刷新成了什么。
*/
State* State::readAchar()
{
	if (cnt >= L.size())
	{
		return NULL;
	}
	char c = L[cnt];
	//	cout<<"cnt is "<<cnt<<" C is "<<c<<endl;
	//	cout<<"now state is "<<this->state<<endl;
	string nextQ = this->toQ[c];
	//	cout<<"nextQ is "<<nextQ<<endl;
	cnt++;
	return &F[nextQ];
}

class StateChange
{
public:
	State *it1, *it2;
	char c;
	DWORD t;
	StateChange()
	{
		t = 200;
	}

	void init()
	{
		it1 = &F[S];
		this->c = L[cnt];
		it2 = it1->readAchar();
	}

	void move()
	{
		if (cnt < L.size())
		{
			it1 = it2;
			this->c = L[cnt];
			it2 = it1->readAchar();
			if (it2->state.size() == 0)
			{
				it2->x = it1->x+100;
				it2->y = it1->y;
				it2->state = "Not recognized";
				cnt = L.size();
				
			}
		}
	}

	void ChangeColor()
	{
		settextcolor(GREEN);
	}

	void RetSetColor()
	{
		settextcolor(BLACK);
	}
};

StateChange SC;

//处理图形的函数------------------------start-------------------------

void outStringxy(int x, int y, string str)
{
	TCHAR s[100];
	stringToTCHAR(str, s, str.size() + 1);
	outtextxy(x, y, s);
}

void init()
{
	//初始化点（状态）
	n = Q.size();
	set<string>::iterator it = Q.begin();
	int dis = (winw - 200) / n;
	map<string, State>::iterator itf = F.begin();
	for (int i = 1; i <= n; i++)
	{
		itf->second.x = i * dis;
		itf->second.y = minh;
		itf->second.r = r;
		itf++;
	}
	//状态转移类SC的初始化，此初始化必须放在DFA初始化完成之后，也就是说放在work函数后
	SC.init();
	//画图形的基本设置初始化

	// 设置背景色为蓝色
	setbkcolor(WHITE);

	setlinecolor(RED);

	setfillcolor(GREEN);

	settextstyle(16, 8, _T("Courier"));	// 设置字体

	// 设置颜色
	settextcolor(BLACK);
	setlinecolor(BLACK);
}

double distence(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

void getState()
{
	itOfSelectedState = F.begin();
	while (itOfSelectedState != F.end())
	{
		if (distence(itOfSelectedState->second.x, itOfSelectedState->second.y, m.x, m.y) < itOfSelectedState->second.r)
		{
			return;
		}
		itOfSelectedState++;
	}
}

void paintArrow(int x, int y, double angle)
{
	//fillcircle(x, y, r / 4);
	double angle1 = angle + pi / 180.0 * 165.0, angle2 = angle1 + pi / 180.0 * 30.0;
	int x1=x+r*cos(angle1), y1=y-r*sin(angle1), x2=x+r*cos(angle2), y2=y-r*sin(angle2);
	line(x, y, x1, y1);
	line(x, y, x2, y2);
}

void stringToTCHAR(string str, TCHAR s[], int len)
{
	#ifdef UNICODE
		_stprintf_s(s, len, _T("%S"), str.c_str());
	#else
		_stprintf_s(s, len, _T("%s"), name.c_str());
	#endif
}

void paintEdge(int x1, int y1, int x2, int y2,char c)
{
	//paintArrow(200, 200, pi / 3*4);
	string name = "";
	name += c;

	double angle = 0;
	/*
	可知DFA，有两种状态转移形式，一种是本状态通过一个字符到达其他状态，另一种是通过一个字符又回到本状态
	针对到达其他状态的设计画带箭头的直线函数
	针对到达本状态的画一个，自回环
	*/
	if (x1 == x2 && y1 == y2)
	{
		circle(x1,y1 - 2 * r,2*r);
		outStringxy(x1, y1-5 * r-5,name);
		int x3 = x2 + r * cos(pi*165.0 / 180.0);
		//注意计算机的xy坐标的正方向，和自己手动画的不一样，y的正方向正相反，所以要减去
		int y3 = y2 - r * sin(pi*165.0 / 180.0);
		//fillcircle(x3, y3, r / 4);
		paintArrow(x3, y3, -1.0*pi/6);
	}
	else
	{
		outStringxy((x1 + x2) / 2, (y1 + y2) / 2, name);
		line(x1, y1, x2, y2);
		//如果在一条竖线上，那么角度不是90度就是270度。
		if (x1 == x2)
		{
			if (y2 > y1)
			{
				angle = pi/2;
			}
			else
			{
				angle = pi / 2 * 3;
			}
		}
		else
		{
			//如果不在一条竖线上，那么就可以算斜率了
			double tangle = (y2 - y1)*1.0 / (x2 - x1)*1.0;
			//单纯的斜率只能表示(-90-90)度，我们需要根据y1，y2的大小将范围扩大到[0-360)
			angle = atan(-tangle);
			if (x2 < x1)
			{
				angle += pi;
			}
		}
		//string aa = std::to_string(cos(angle))+"    "+std::to_string(angle)+"    "+std::to_string(pi/3);
		//TCHAR tt[500];
		//stringToTCHAR(aa, tt, aa.size()+1);
		//outtextxy(200,200, tt);
		//paintArrow(300, 300, pi / 3);
		paintArrow(x2 + r * cos(angle+pi), y2 - r * sin(angle+pi), angle);
	}
}

void paintLanguage()
{
	string t = "a";
	for (int i = 0; i != L.size(); i++)
	{
		t[0] = L[i];
		if (i == cnt-1)
		{
			settextcolor(GREEN);
		}
		else
		{
			settextcolor(BLACK);
		}
		outStringxy(200+i*10, 150, t);
	}
	settextcolor(GREEN);
	t = "";
	if (SC.it1->state.size()!=0)
	{
		t += SC.it1->state;
	}
	else
	{
		t += "NULL";
	}
	t += " --> ";
	if (SC.it2->state.size() != 0)
	{
		t += SC.it2->state;
	}
	else
	{
		t += "NULL";
	}

	outStringxy(400, 150, t);
	t = "1";
	t[0] = SC.c;
	outStringxy(400+SC.it1->state.size()+20, 135, t);
	settextcolor(BLACK);
}

void paint()
{
	// 用背景色清空屏幕
	cleardevice();

	/*
	画边，即状态转移方程
	*/
	itOfSelectedState = F.begin();
	while (itOfSelectedState != F.end())
	{
		bool flag = false;
		map<char, string>::iterator eit=itOfSelectedState->second.toQ.begin();
		while (eit != itOfSelectedState->second.toQ.end())
		{
			if (SC.it1!=NULL&&SC.it2!=NULL&&SC.it1->state == itOfSelectedState->first&&SC.c == eit->first&&SC.it2->state == eit->second)
			{
				settextcolor(BLUE);
				setlinecolor(BLUE);
			}
			else
			{
				settextcolor(BLACK);
				setlinecolor(BLACK);
			}
			paintEdge(itOfSelectedState->second.x, itOfSelectedState->second.y, F[eit->second].x, F[eit->second].y,eit->first);
			eit++;
		}
		itOfSelectedState++;
	}

	//画状态点，并在状态点上添加该状态的名字
	itOfSelectedState = F.begin();
	while (itOfSelectedState != F.end())
	{
		bool flag = false;
		if ((SC.it1 != NULL && SC.it1->state == itOfSelectedState->first) || (SC.it2 != NULL && SC.it2->state == itOfSelectedState->first))
		{
			flag = true;
		}
		else
		{
			flag = false;
		}
		if (flag)
		{
			setfillcolor(BLUE);
			settextcolor(GREEN);
		}
		else
		{
			setfillcolor(GREEN);
			settextcolor(BLACK);
		}
		fillcircle(itOfSelectedState->second.x, itOfSelectedState->second.y, itOfSelectedState->second.r);
		string name = itOfSelectedState->second.state;


		outStringxy(itOfSelectedState->second.x - itOfSelectedState->second.r / 2, itOfSelectedState->second.y - itOfSelectedState->second.r / 2,name);

		itOfSelectedState++;
	}

	paintLanguage();
}

//处理图形的函数------------------------end-------------------------


//处理形式语言的函数-------------------start----------------------------

vector<string> split(string str)
{
	vector<string> vst;
	string t;
	for (int i = 0; i != str.size(); i++)
	{
		if (str[i] == ' ')
		{
			if (t != "")
			{
				vst.push_back(t);
				t = "";
			}
		}
		else
		{
			t += str[i];
		}
	}
	if (t != "")vst.push_back(t);
	return vst;
}

vector<string> splitF(string buff)
{
	vector<string> vs;
	string t = "";
	int cnt = 0;
	//读取起始状态 
	while (buff[cnt] != ' ')
	{
		t += buff[cnt];
		cnt++;
	}
	vs.push_back(t);

	//读取通过的字符
	while (buff[cnt] == ' ')cnt++;
	t = buff[cnt];
	vs.push_back(t);
	while (buff[cnt] != '>')cnt++;
	cnt++;

	t = "";
	//读取到达的状态集 
	while (cnt < buff.size())
	{
		if (buff[cnt] == ' ')
		{
			if (t.size())
			{
				vs.push_back(t);
			}
			t = "";
		}
		else
		{
			t += buff[cnt];
		}
		cnt++;
	}
	if (t.size())
	{
		vs.push_back(t);
	}
	return vs;
}

void readFile()
{
	ifstream infile;
	infile.open(Path.c_str());
	if (!infile)
	{
		ofstream outfile;
		outfile.open(Path.c_str());
		outfile.close();
		infile.open(Path.c_str());
	}

	//读取字符集 
	std::getline(infile, buff);
	vector<string> vst = split(buff);
	vector<string>::iterator vit = vst.begin();
	while (vit != vst.end())
	{
		//cout<<*vit<<endl;
		E.insert((*vit)[0]);
		vit++;
	}

	//读取状态集合 
	std::getline(infile, buff);
	vst = split(buff);
	vit = vst.begin();
	while (vit != vst.end())
	{
		Q.insert((*vit));
		vit++;
	}

	//读取开始状态
	std::getline(infile, buff);
	S = buff;

	//读取终止状态集合
	std::getline(infile, buff);
	vst = split(buff);
	vit = vst.begin();
	while (vit != vst.end())
	{
		T.insert((*vit));
		vit++;
	}

	//读取状态转移函数 
	/*
	首先应该先去F中查看是否已经储存过这个状态了，现在只是要在该状态下加一个转移函数
	还是这个状态还不存在，我们需要加一个状态到F中。
	*/
	while (std::getline(infile, buff))
	{
		//cout<<"buff is "<<buff<<endl;
		vst = splitF(buff);
		vit = vst.begin();
		//开始状态 
		string state = *vit;
		vit++;
		//通过的字符 
		char c = (*vit)[0];
		vit++;
		string toQ = *vit;

		State st = F[state];
		if (st.state.size() != 0)//如果已经存在该状态了，那么要再该状态下加一个状态转移函数 
		{
			//cout<<"yi cun zai "<<buff<<endl;
			F[state].toQ[c] = toQ;
		}
		else
		{
			st.state = state;
			st.toQ[c] = toQ;
			F[st.state] = st;
		}
	}
	infile.close();
}

void readL()
{
	ifstream infile;
	infile.open("./L.txt");
	if (!infile)
	{
		ofstream outfile;
		outfile.open(Path.c_str());
		outfile.close();
		infile.open(Path.c_str());
	}

	std::getline(infile, L);

	infile.close();
}

void work()
{
	readFile();
	readL();
	//	cout<<L<<endl;

	//	cout<<"F size is"<<F.size()<<endl;

	//cout << F[S].readAchar() << endl;

	//	map<string,State>::iterator fit=F.begin();
	//	
	//	cout<<"F size is"<<F.size()<<endl;
	//	
	//	while(fit!=F.end())
	//	{
	//		cout<<"key is "<<fit->first<<" value is "<<fit->second.state<<endl;
	//		fit++;
	//	}
	//	
}

//处理形式语言的函数-------------------end----------------------------

int main()
{
	//读取DFA，句子，初始化DFA。
	work();
	// 初始化图形窗口
	initgraph(winw, winh);
	//初始化图形及图形的相关设置
	init();
	//画出图形
	paint();

	bool RB = false;

	DWORD k = ::GetTickCount();

	while (true)
	{
		//获取一条鼠标消息
		m = GetMouseMsg();
		//如果鼠标左键按下，说明当前用户是否在拖动我们的状态点，如果在拖动我们就更新坐标并重画画板。
		if (m.mkLButton)
		{
			//获取当前鼠标选中的状态点，保存该点的指针（迭代器）到itOfSelectedState中
			getState();
			//如果通过getState函数找到了当前用户想要拖动的状态点，那么我们就把selected置为1
			if (itOfSelectedState != F.end())
			{
				//如果有状态点被选中了，那么持续更新该状态点的位置信息
				itOfSelectedState->second.x = m.x;
				itOfSelectedState->second.y = m.y;
			}
			//每20毫秒重画一次画板
			if (::GetTickCount() - k > 20)
			{
				//GetTickCount返回（retrieve）从操作系统启动所经过（elapsed）的毫秒数，它的返回值是DWORD。
				paint();
				k = ::GetTickCount();
			}
		}
		if (m.mkRButton)
		{
			RB = true;
		}
		else
		{
			if (RB)
			{
				RB = false;
				SC.move();
				//outStringxy(200, 200, SC.it1->state + ":" + SC.c + ":" + SC.it2->state);
				paint();
			}
		}

		/*
		if(_kbhit())
		{
			char c = _getch();
			if (c == 's')
			{
				//SC.ChangeColor1();
			}
			string str = "s";
			str += c;
			str+="e";
			outStringxy(300, 300, str);
		}
		*/
		//Sleep(sleep_time);
	}
	// 关闭图形窗口
	closegraph();
	return 0;
}

