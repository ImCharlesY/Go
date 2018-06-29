
// ChessView.cpp : CChessView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Chess.h"
#endif

#include "ChessDoc.h"
#include "ChessView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChessView

IMPLEMENT_DYNCREATE(CChessView, CView)

BEGIN_MESSAGE_MAP(CChessView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CChessView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
//	ON_WM_RBUTTONUP()
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CChessView 构造/析构

CChessView::CChessView()
	:Turn(0),Over(0),GameOver(&Board),AI(&Board),cur(vec(-1,-1)),m_pThread(NULL)
{
	// TODO: 在此处添加构造代码
}

CChessView::~CChessView()
{
}

BOOL CChessView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CChessView 绘制

void CChessView::OnDraw(CDC* pDC)
{
	CChessDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	// ===== board
	CRect rectClient;
    GetClientRect(&rectClient);
    pDC->FillSolidRect(&rectClient, RGB(216,186,156));

	CBitmap bm;
    bm.LoadBitmap(IDB_BITMAP1);
    CBrush brush;
    brush.CreatePatternBrush(&bm);
	CRect rectBG(90,90,670,670);
    pDC->FillRect(&rectBG,&brush);

	for (int i = 0; i < MyBoard::_size; ++i)
	{
		pDC->MoveTo(100, 100 + BLen * i);
		pDC->LineTo(100 + (MyBoard::_size - 1) * BLen, 100 + BLen * i);
		pDC->MoveTo(100 + BLen * i, 100);
		pDC->LineTo(100 + BLen * i, 100 + (MyBoard::_size - 1) * BLen);
	}

	const int lenOfP = BLen / 12;
	const int initNum1 = ((MyBoard::_size - 1) / 2 - 1) / 2;
	const int initNum2 = ((MyBoard::_size - 1) / 2 + 1) / 2;

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			CBrush Bbrush(RGB(0, 0, 0));
			auto old = pDC->SelectObject(Bbrush);

			int pb = 100 + (initNum1 + initNum2 * i) * BLen;
			int pe = 100 + (initNum1 + initNum2 * j) * BLen;
			pDC->Rectangle(pb - lenOfP, pe - lenOfP, pb + lenOfP, pe + lenOfP);
			pDC->SelectObject(old);
		}
	// ===== board end

	// ===== chess
	const int lenOfChess = BLen / 2 - 3;
	for (int i = 0; i < MyBoard::_size; ++i)
		for (int j = 0; j < MyBoard::_size; ++j)
			if (Board.Get(vec(i, j)) == black)
			{
				CPen BPen(PS_SOLID,1,RGB(0,0,0));
				CBrush Bbrush(RGB(0, 0, 0));
				pDC->SelectObject(&BPen);
				pDC->SelectObject(&Bbrush);
				int pb = 100 + BLen * i;
				int pe = 100 + BLen * j;
				pDC->Ellipse(pb - lenOfChess, pe - lenOfChess, pb + lenOfChess, pe + lenOfChess);
			}
			else if (Board.Get(vec(i, j)) == white)
			{
				CPen WPen(PS_SOLID,1,RGB(255,255,255));
				CBrush Wbrush(RGB(255, 255, 255));
				pDC->SelectObject(&WPen);
				pDC->SelectObject(&Wbrush);
				int pb = 100 + BLen * i;
				int pe = 100 + BLen * j;
				pDC->Ellipse(pb - lenOfChess, pe - lenOfChess, pb + lenOfChess, pe + lenOfChess);
			}

	CPen pen0(PS_SOLID,1,RGB(0,0,0));
	pDC->SelectObject(pen0);
	pDC->MoveTo(550,690);
	pDC->LineTo(610,690);
	pDC->MoveTo(550,711);
	pDC->LineTo(610,711);
	pDC->MoveTo(550,690);
	pDC->LineTo(550,711);
	pDC->MoveTo(610,690);
	pDC->LineTo(610,711);
    pDC->FillSolidRect(CRect(550,690,610,711), RGB(237,239,252));
	pDC->TextOutW(558,691,_T("UnDo"));

	pDC->MoveTo(150,690);
	pDC->LineTo(210,690);
	pDC->MoveTo(150,711);
	pDC->LineTo(210,711);
	pDC->MoveTo(150,690);
	pDC->LineTo(150,711);
	pDC->MoveTo(210,690);
	pDC->LineTo(210,711);
	pDC->FillSolidRect(CRect(150,690,210,711), RGB(237,239,252));
	pDC->TextOutW(160,691,_T("Start"));

	if(cur!=vec(-1,-1))
	{
		CPen pen(PS_SOLID,1,RGB(255,0,0));
		pDC->SelectObject(pen);
		pDC->MoveTo(100 + BLen * cur.first - 4,100 + BLen * cur.second);
		pDC->LineTo(100 + BLen * cur.first + 4,100 + BLen * cur.second);
		pDC->MoveTo(100 + BLen * cur.first,100 + BLen * cur.second - 4);
		pDC->LineTo(100 + BLen * cur.first,100 + BLen * cur.second + 4);
	}
	// ===== chess end
}


// CChessView 打印


void CChessView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CChessView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CChessView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CChessView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

//void CChessView::OnRButtonUp(UINT /* nFlags */, CPoint point)
//{
//	ClientToScreen(&point);
//	OnContextMenu(this, point);
//}

void CChessView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CChessView 诊断

#ifdef _DEBUG
void CChessView::AssertValid() const
{
	CView::AssertValid();
}

void CChessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChessDoc* CChessView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChessDoc)));
	return (CChessDoc*)m_pDocument;
}
#endif //_DEBUG


// CChessView 消息处理程序


void CChessView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CView::OnPaint()
	OnDraw(&dc);
}

int cnt = 0;

void CChessView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	srand(time(NULL));
	vec pos((point.x - 100 + BLen / 2) / BLen, (point.y - 100 + BLen / 2) / BLen);
	if (!Over && point.x < 610 && point.x > 550 && point.y > 690 && point.y < 711)
	{
		DWORD dwExitCode=0;
		if(m_pThread)
			if(GetExitCodeThread(m_pThread->m_hThread,&dwExitCode) && dwExitCode==STILL_ACTIVE)  return;
		Board.UnDo();
		cur = vec(-1,-1);
		Invalidate(TRUE);
		cnt -= 2;
		if (cnt < 0) cnt = 0;
		OnPaint();
	}
	else if (point.x < 210 && point.x > 150 && point.y > 690 && point.y < 711)
	{
		DWORD dwExitCode=0;
		if(m_pThread)
			if(GetExitCodeThread(m_pThread->m_hThread,&dwExitCode) && dwExitCode==STILL_ACTIVE) 
		{
			TerminateThread(m_pThread->m_hThread,-1);  
			CloseHandle(m_pThread->m_hThread); 
			m_pThread->Delete();
			m_pThread=NULL;
		}
		Board.Clear();
		cnt = 0;
		cur = vec(-1,-1);
		Over = 0;
		Turn = 0;
		Invalidate(TRUE);
	}
	if(Turn == 0 && !Over && Board.Set(pos, black))
	{
		DWORD dwExitCode=0;
		if(m_pThread)
			if(GetExitCodeThread(m_pThread->m_hThread,&dwExitCode) && dwExitCode==STILL_ACTIVE) return;
		++cnt;
		Invalidate(TRUE);
		OnPaint();
		if (GameOver(pos))
		{
			Over = 1;
			Turn = 0;
			AfxMessageBox(_T("Congratulations! You win!"));
			Invalidate(TRUE);
		}
		else
		{
			Turn = 1;
			/*if (cnt <= 6) cur = pos = AI(4);
			else cur = pos = AI(8,2.0);
			Board.Set(pos, white);
			Turn = 0;
			++cnt;
			Invalidate(TRUE);
			if (GameOver(pos)) 
			{
				Over = 1;
				Turn = 0;
				AfxMessageBox(_T("You lose!"));
				Invalidate(TRUE);
			}*/
			if (cnt > 6) 
			{
				m_pThread = AfxBeginThread(ThreadFunc,this);
			}
			else
			{
				cur = AI(4);
				Board.Set(cur, white);
				Turn = 0;
				++cnt;
				Invalidate(TRUE);
				if (GameOver(cur)) 
				{
					Over = 1;
					Turn = 0;
					AfxMessageBox(_T("You lose!"));
					Invalidate(TRUE);
				}
			}
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

UINT ThreadFunc(LPVOID pParm)
{
	auto p = (CChessView*)pParm;
	p->cur = p->AI(8,2.0);
	p->Board.Set(p->cur, white);
	p->Turn = 0;
	++cnt;
	p->Invalidate(TRUE);
	if (p->GameOver(p->cur)) 
	{
		p->Over = 1;
		p->Turn = 0;
		AfxMessageBox(_T("You lose!"));
		p->Invalidate(TRUE);
	}
	return 0;
}