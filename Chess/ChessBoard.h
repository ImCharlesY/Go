#ifndef _BOARD_
#define _BOARD_

#include <fstream>
#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <random>
#include <functional>
#include <climits>
#include <cstdlib>
#include <ctime>

// ===== 类型重命名
typedef std::pair<int, int> vec;
typedef unsigned long long U64;
// ===== 类型重命名结束

// ===== 散列表定义部分
#define hashfEMPTY 0
#define hashfEXACT 1
#define hashfALPHA 2
#define hashfBETA 3
#define valUNKNOWN _I32_MAX
typedef struct tagHASHE 
{
	int depth;
	int flags;
	long long value;
	tagHASHE():depth(0),flags(0),value(0) { }
} HASHE;

// ===== 散列表定义部分结束

// ========== 棋盘模块 ==========

enum Status {blank,black,white};
class BoardIterator;
class MyBoard
{
	friend class BoardIterator;
	std::vector<std::vector<Status>> _board;
	std::vector<std::vector<U64>> HashOfB, HashOfW;
	U64 _ZobristKey;
	std::map<U64, HASHE> hash;
	std::queue<vec> Record;
	int _Cnt;
public:
	enum { _size = 15 };
	MyBoard();
	~MyBoard();
	bool Set(vec pos, Status color);
	void tmpSet(vec pos, Status color);
	void UnSet(vec pos);
	Status & Get(vec pos);
	U64 ZobristKey()const { return _ZobristKey; }
	int Cnt()const { return _Cnt; }
	bool UnDo();
	void Clear();
	long long ProbeHash(int depth, long long alpha, long long beta);
	void RecordHash(int depth, long long val, int hashf);
};

// ==============================

// ========== 迭代器 ==========

class BoardIterator
{
public:
	enum TypeOfItr { Tra, UD, RL, DeL, DeR };

	BoardIterator(MyBoard *s) :self(s), curr(vec(0, 0)), TOI(UD) { }

	Status & operator*() { return self->Get(curr); }
	vec operator&() { return curr; }
	bool operator+()const
	{
		return (curr.first < 0 || curr.first > self->_size - 1
			|| curr.second < 0 || curr.second > self->_size - 1) ? false : true;
	}

	TypeOfItr SetTypeOfItr(TypeOfItr s = UD) { TypeOfItr tmp = TOI; TOI = s; return tmp; }
	vec SetCurrPos(const vec &pos) { vec tmp = curr; curr = pos; return tmp; }

	void operator++()
	{
		switch (TOI)
		{
		case Tra:if (++curr.first == 15) ++curr.second,curr.first = 0;
			break;
		case UD:if (operator+()) --curr.second;
			break;
		case RL:if (operator+()) ++curr.first;
			break;
		case DeL:if (operator+()) --curr.second, --curr.first;
			break;
		case DeR:if (operator+()) --curr.second, ++curr.first;
			break;
		}
	}
	void operator++(int) { operator++(); }
	void operator--()
	{
		switch (TOI)
		{
		case Tra:if (--curr.first == -1) --curr.second, curr.first = 15 - 1;
			break;
		case UD:if (operator+()) ++curr.second;
			break;
		case RL:if (operator+()) --curr.first;
			break;
		case DeL:if (operator+()) ++curr.second, ++curr.first;
			break;
		case DeR:if (operator+()) ++curr.second, --curr.first;
			break;
		}
	}
	void operator--(int) { operator--(); }

private:
	MyBoard *self;
	vec curr;
	TypeOfItr TOI;
};

// ==============================

// ========== 判断模块 ==========

class Judge
{
	MyBoard *self;
	BoardIterator Itr;

	int Check(const vec &lastest);

public:
	Judge(MyBoard *s):self(s),Itr(s) { }
	bool operator()(const vec &lastest);
};

// ==============================

#endif