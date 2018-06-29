
// ChessView.h : CChessView 类的接口
//

#pragma once

#include "ChessBoard.h"
#include "AI.h"
#define BLen (40)



class CChessView : public CView
{
protected: // 仅从序列化创建
	CChessView();
	DECLARE_DYNCREATE(CChessView)

// 特性
public:
	CChessDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CChessView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CWinThread *m_pThread;

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	bool Turn;
	bool Over;
	MyBoard Board;
	Judge GameOver;
	AIChess AI;
	vec cur;
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void OnDrawWhileThread(CDC* pDC);
};

#ifndef _DEBUG  // ChessView.cpp 中的调试版本
inline CChessDoc* CChessView::GetDocument() const
   { return reinterpret_cast<CChessDoc*>(m_pDocument); }
#endif

UINT ThreadFunc(LPVOID pParm);