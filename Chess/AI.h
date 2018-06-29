#ifndef _AI_
#define _AI_

#include "ChessBoard.h"

struct Issue	//事件结构：储存事件所在位置及其优先级
{
	vec Spot;	//事件所在位置
	long long Pr;			//事件优先级
	Issue(vec v, long long t) :Spot(v), Pr(t) { }
	bool operator>(const Issue &R)const { return Pr > R.Pr; }
};

class Priority	//定义函数符，提供给优先队列进行优先级的确定
{
public:
	bool operator()(const Issue &I1, const Issue & I2)
	{
		return I1.Pr < I2.Pr;
	}
};

class AIChess
{

	MyBoard *self;
	BoardIterator Itr;

	// ======= 局面判断
	int Check(const vec &lastest);
	bool operator()(const vec &lastest);
	// ======= 局面判断结束

	//=========某位置附近形势查看
	int Situation(const vec &Curr, Status color);
	Issue Analyse(vec Curr);
	//====================================================
	
	void GenerateBlankQueue(std::vector<vec> &Candidate);
	long long ScoreTable(int MaxSus, int NumOfBlk);
	long long GetValueOfLine(std::vector<vec> &InitialPos, Status color);
	long long GetValue(Status color);
	long long NegaMaxAlphaBeta(vec Curr, long long alpha, long long beta, int depth, Status color);

public:
	AIChess(MyBoard *s) :self(s), Itr(s) { }
	vec operator()(int depth, double time = 2.0);
};

#endif // !_AI_