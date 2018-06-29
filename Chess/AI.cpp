#include "stdafx.h"
#include "AI.h"

HANDLE hMutex1 = CreateMutex(NULL,FALSE,NULL);	//棋盘锁
HANDLE hMutex2 = CreateMutex(NULL,FALSE,NULL);	//置换表锁
HANDLE hMutex3 = CreateMutex(NULL,FALSE,NULL);

// ======= 局面判断
int AIChess::Check(const vec &lastest)
{
	auto oldPos = Itr.SetCurrPos(lastest);
	Status CurColor(*Itr);		//得到当前位置状态标记
	int MaxSeq(0);						//记录最大连子数

	for (; +Itr; ++Itr)	//往前检索
		if (*Itr == CurColor && ++MaxSeq <= 5);
		else break;

		Itr.SetCurrPos(lastest);
		for (; +Itr; --Itr)	//往后检索
			if (*Itr == CurColor && ++MaxSeq <= 5);
			else break;

			Itr.SetCurrPos(oldPos);
			return MaxSeq - 1;
}
bool AIChess::operator()(const vec &lastest)
{
	int MaxSeq(0);			//记录最大连子数
	auto old = Itr.SetTypeOfItr(BoardIterator::TypeOfItr(0));
	for (int i = 1; i <= 4; ++i)		//分别调用四种迭代器对当前位置周围进行检查
	{
		Itr.SetTypeOfItr(BoardIterator::TypeOfItr(i));
		MaxSeq = max(MaxSeq, Check(lastest));	//记录当前位置周围连子最长的长度
	}
	Itr.SetTypeOfItr(old);
	return (MaxSeq < 5) ? 0 : 1;
}
// ======= 局面判断结束

//=========某位置附近形势查看
int AIChess::Situation(const vec &Curr, Status color)
{
	int MaxSus(1), NumOfRes(0);		//本方最长连子数，同一方向上被封锁端数（0，1，2）

	auto oldPos = Itr.SetCurrPos(Curr);
	for (++Itr; +Itr; ++Itr)
		if (*Itr == color) ++MaxSus;
		else break;
		//查看循环结束原因，如果是遇到边界或者遇到黑棋，视为被封锁
		if (!+ Itr || *Itr == ((color == black) ? white : black)) ++NumOfRes;

		//反向搜索，原理同上
		Itr.SetCurrPos(Curr);
		for (--Itr; +Itr; --Itr)
			if (*Itr == color) ++MaxSus;
			else break;
			//查看循环结束原因，如果是遇到边界或者遇到黑棋，视为被封锁
			if (!+ Itr || *Itr == ((color == black) ? white : black)) ++NumOfRes;

			Itr.SetCurrPos(oldPos);
			//除了死五外所有死棋以及仅当前位置一棋两种情况均为最低优先级
			if ((MaxSus < 5 && NumOfRes == 2) || MaxSus == 1) return 0;
			return (color == white) ? MaxSus * 10 + 3 - NumOfRes : MaxSus * 10 + 3 - NumOfRes;
}
Issue AIChess::Analyse(vec Curr)
{
	int MaxPriority(0);			//记录该位置周围某方向最高优先级事件的优先级
	int Priorities[4] = { 0 };	//记录该位置周围四个方向各自最高优先级事件的优先级
	int i(0);
	auto old = Itr.SetTypeOfItr(BoardIterator::TypeOfItr(0));
	for (int j = 1; j <= 4; ++j)		//分别调用四种迭代器对当前位置周围进行检查
	{
		Itr.SetTypeOfItr(BoardIterator::TypeOfItr(j));
		Priorities[i] = max(Situation(Curr, white), Situation(Curr, black));	//记录当前方向事件的优先级
		MaxPriority = max(MaxPriority, Priorities[i]);	//记录当前位置各方向中事件的最高优先级
		++i;
	}
	Itr.SetTypeOfItr(old);
	int SumOfPrior(0);		//四个方向事件优先级总和
	for (int i = 0; i < 4; ++i) SumOfPrior += Priorities[i];

	int FinalPrior = MaxPriority * 4;
	if (MaxPriority <= 36) FinalPrior = max(FinalPrior, SumOfPrior);

	return Issue(Curr, FinalPrior);	//将当前位置及最高优先级包装成Issue对象返回
}
//====================================================

void AIChess::GenerateBlankQueue(std::vector<vec> &Candidate)
{
	/*首先，清空优先队列；
	然后遍历棋盘（从0，0开始），对每一个状态为空（blank）的位置进行检查，
	检查使用遍历器进行，获得该位置可能发生事件的最高优先级，
	最后将信息包装成一个Issue对象后送入优先队列
	*/
	std::priority_queue<Issue, std::vector<Issue>, Priority> PqOfIssue;
	auto oldType = Itr.SetTypeOfItr(BoardIterator::TypeOfItr(0));
	auto oldPos = Itr.SetCurrPos(vec(0, 0));
	for (; +Itr; ++Itr)		//遍历检索棋盘
		if (*Itr == blank)
			PqOfIssue.push(Analyse(&Itr));
	Itr.SetTypeOfItr(oldType);
	Itr.SetCurrPos(oldPos);

	while (!PqOfIssue.empty() && Candidate.size() < 16)
	{
		Candidate.push_back(PqOfIssue.top().Spot);
		PqOfIssue.pop();
	}
}

long long AIChess::ScoreTable(int MaxSus, int NumOfBlk)//计分板
{
	if (MaxSus >= 5) return 500000;				//成五
	else if (MaxSus == 4)
	{
		if (NumOfBlk == 2) return 200000;		//活四
		else if (NumOfBlk == 1) return 10000;	//死四
	}
	else if (MaxSus == 3)
	{
		if (NumOfBlk == 2) return 6000;			//活三
		else if (NumOfBlk == 1) return 800;		//死三
	}
	else if (MaxSus == 2)
	{
		if (NumOfBlk == 2) return 400;			//活二
		else if (NumOfBlk == 1) return 90;		//死二
	}
	else if (MaxSus == 1)
	{
		if (NumOfBlk == 2) return 20;			//活一
		else if (NumOfBlk == 1) return 4;		//死一
	}
	return 0;
}

long long AIChess::GetValueOfLine(std::vector<vec> &InitialPos, Status color)
{
	long long FinalValue(0);
	auto old = Itr.SetCurrPos(vec(0, 0));
	for (auto IP = InitialPos.begin(); IP != InitialPos.end(); ++IP)
	{
		int MaxSus(0), NumOfBlk(0);

		//获得白方得分
		long long tmpValueOfW(0);
		Itr.SetCurrPos(*IP);
		for (; +Itr; ++Itr)
		{
			if (*Itr == white) ++MaxSus;
			else if (*Itr == blank)
			{
				if (MaxSus != 0)
				{
					tmpValueOfW += ScoreTable(MaxSus, NumOfBlk + 1);
					MaxSus = 0;
				}
				NumOfBlk = 1;
			}
			else
			{
				tmpValueOfW += ScoreTable(MaxSus, NumOfBlk);
				MaxSus = 0;
				NumOfBlk = 0;
			}
		}
		tmpValueOfW += ScoreTable(MaxSus, NumOfBlk);

		MaxSus = 0, NumOfBlk = 0;

		//获得黑方得分
		long long tmpValueOfB(0);
		Itr.SetCurrPos(*IP);
		for (; +Itr; ++Itr)
		{
			if (*Itr == black) ++MaxSus;
			else if (*Itr == blank)
			{
				if (MaxSus != 0)
				{
					tmpValueOfB += ScoreTable(MaxSus, NumOfBlk + 1);
					MaxSus = 0;
				}
				NumOfBlk = 1;
			}
			else
			{
				tmpValueOfB += ScoreTable(MaxSus, NumOfBlk);
				MaxSus = 0;
				NumOfBlk = 0;
			}
		}
		tmpValueOfB += ScoreTable(MaxSus, NumOfBlk);

		if (color == white)
			FinalValue += tmpValueOfW - tmpValueOfB;
		else
			FinalValue += tmpValueOfB - tmpValueOfW;
	}
	Itr.SetCurrPos(old);
	return FinalValue;
}

long long AIChess::GetValue(Status color)
{
	long long Value(0);

	auto old = Itr.SetTypeOfItr(BoardIterator::TypeOfItr(0));
	//分别调用四种迭代器
	for (int j = 1; j <= 4; ++j)
	{
		Itr.SetTypeOfItr(BoardIterator::TypeOfItr(j));

		//获得当前迭代器遍历所需的初始位置的列表
		std::vector<vec> InitialPos;
		switch (j)
		{
		case 1:
			for (int i(0); i < MyBoard::_size; ++i) InitialPos.push_back(vec(i, MyBoard::_size - 1));
			break;
		case 2:
			for (int i(0); i < MyBoard::_size; ++i) InitialPos.push_back(vec(0, i));
			break;
		case 3:
			for (int i(0); i < MyBoard::_size; ++i) InitialPos.push_back(vec(i, MyBoard::_size - 1));
			for (int i(0); i < MyBoard::_size - 1; ++i) InitialPos.push_back(vec(MyBoard::_size - 1, i));
			break;
		case 4:
			for (int i(0); i < MyBoard::_size; ++i) InitialPos.push_back(vec(i, MyBoard::_size - 1));
			for (int i(0); i < MyBoard::_size - 1; ++i) InitialPos.push_back(vec(0, i));
			break;
		}

		Value += GetValueOfLine(InitialPos, color);
	}
	Itr.SetTypeOfItr(old);
	return Value;
}

long long AIChess::NegaMaxAlphaBeta(vec Curr, long long alpha, long long beta, int depth, Status color)
{
	long long value = 0;         //当前结点价值
	bool FoundPV = false;       //是否找到PV结点标记

	int hashf = hashfALPHA;
	
	if ((value = self->ProbeHash(depth, alpha, beta)) != valUNKNOWN) return value;

	if (depth == 0 || operator()(Curr))	//如果已经递归到底层
	{
		value = GetValue(color);
		self->RecordHash(depth, value, hashfEXACT);
		return value;
	}

	//产生一个序列，将所有有价值的空位记录起来放进队列
	std::vector<vec> Candidate;
	GenerateBlankQueue(Candidate);

	for (int i(0); i < Candidate.size(); ++i)	//遍历每一个空位，获取每一个空位的价值，选取其中的最大价值
	{
		self->tmpSet(Candidate[i], color); //先选取该位置下一子	

		if (FoundPV)    //如果已经找到PV结点，则对其他结点，缩小期望窗口查询
		{
			value = -NegaMaxAlphaBeta(Candidate[i], -alpha - 1, -alpha, depth - 1, ((color == black) ? white : black));
			if (value > alpha && value < beta)
				value = -NegaMaxAlphaBeta(Candidate[i], -beta, -alpha, depth - 1, ((color == black) ? white : black));
		}
		else
			value = -NegaMaxAlphaBeta(Candidate[i], -beta, -alpha, depth - 1, ((color == black) ? white : black));

		self->UnSet(Candidate[i]);		//该位置重新置空

		if (value >= beta)  //发生beta剪枝
		{
			self->RecordHash(depth, beta, hashfBETA);
			return beta;
		}
		if (value > alpha)
		{
			hashf = hashfEXACT;
			alpha = value;
			FoundPV = true;
		}
	}
	self->RecordHash(depth, alpha, hashf);
	return alpha;
}

vec AIChess::operator()(int depth, double time)
{
	std::vector<vec> Candidate;
	GenerateBlankQueue(Candidate);

	long long bestValue(_I32_MIN);

	auto start = clock();

	std::vector<Issue> tmp;

	int deprec = 0;

	for (int dep = 1; dep <= depth; ++dep)
	{
		deprec = dep;
		tmp.clear();
		for (int i(0); i < Candidate.size(); ++i)	//遍历每一个空位，获取每一个空位的价值，选取其中的最大价值
		{
			self->tmpSet(Candidate[i], white); //先选取该位置下一子	

			long long tmpValue = -NegaMaxAlphaBeta(Candidate[i], _I32_MIN, _I32_MAX, dep - 1, black);	//计算该位置价值

			if (tmpValue > bestValue)
				bestValue = tmpValue;

			tmp.push_back(Issue(Candidate[i], tmpValue));

			self->UnSet(Candidate[i]);		//该位置重新置空
		}
		std::sort(tmp.begin(), tmp.end(), std::greater<Issue>());
		Candidate.clear();
		for (auto it = tmp.begin(); it != tmp.end(); ++it) Candidate.push_back((*it).Spot);

		if ((double)(clock() - start) / CLOCKS_PER_SEC >= time) break;
		if (bestValue >= 200000) break;
	}

#ifdef _DEBUG_
	CString out(_T("Search depth:"));
	out+=(char)('0'+deprec);
	AfxMessageBox(out);
#endif

	int bestIndex = 1;
	while (bestIndex < tmp.size() && tmp[bestIndex].Pr == bestValue) 
		++bestIndex;

	return Candidate[rand()%bestIndex];
}

