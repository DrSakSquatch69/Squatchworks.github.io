#include "DrawingPanel.h"
#include "MainWindow.h"
#include "wx/graphics.h"
#include "wx/dcbuffer.h"
#include <vector>
#include <string>

wxBEGIN_EVENT_TABLE(DrawingPanel, wxPanel)
	EVT_PAINT(DrawingPanel::OnPaint)
	EVT_LEFT_UP(DrawingPanel::MouseClick)
wxEND_EVENT_TABLE()


DrawingPanel::DrawingPanel(wxFrame* parent, wxSize size, std::vector<std::vector<bool>>& board, std::vector<std::vector<int>>& NeighborsCount, Settings& iSettings)
	: wxPanel(parent, wxID_ANY, wxPoint(0, 0), size), rGameBoard(board) , rNeighborsCount(NeighborsCount), rSettings(iSettings)
{
	mParent = parent;
	
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

}


DrawingPanel::~DrawingPanel()
{

}

void DrawingPanel::OnPaint(wxPaintEvent&)
{
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	wxGraphicsContext* pDrawingSurface = wxGraphicsContext::Create(dc);
	if (!pDrawingSurface) { return; }
	
	if (rSettings.GetShowingGrid() == true)
	{
		pDrawingSurface->SetPen(rSettings.GetGridColor());
	}
	else
	{
		pDrawingSurface->SetPen(wxTransparentColor);
	}

	pDrawingSurface->SetBrush(rSettings.GetDeadColor());
	pDrawingSurface->SetFont(wxFontInfo(16), *wxRED);


	//grid

	wxWindow* parentOfDrawingSurface = pDrawingSurface->GetWindow();
	float panelWidth = GetSize().x / (float)rSettings.mGridSize;
	float panelHeight = GetSize().y / (float)rSettings.mGridSize;


	for (int i = 0; i < rSettings.mGridSize; i++)
	{
		int x = i * panelWidth;
		for (int j = 0; j < rSettings.mGridSize; j++)
		{
			pDrawingSurface->SetBrush(rSettings.GetDeadColor());
			int y = j * panelHeight;
			bool cell = CheckCell(i, j);
			if (cell == true)
				pDrawingSurface->SetBrush(rSettings.GetLivingColor());
			pDrawingSurface->DrawRectangle(x, y, panelWidth, panelHeight);
			
			if (rNeighborsCount[i][j] > 0 && rSettings.GetNeighborCountOnOrOff() == true)
			{
				wxString thisCellsNeighborCount = std::to_string(rNeighborsCount[i][j]);
				double textWidth, textHeight;
				pDrawingSurface->GetTextExtent(thisCellsNeighborCount, &textWidth, &textHeight);
				pDrawingSurface->DrawText(thisCellsNeighborCount, x + panelWidth / 2 - textWidth, y + panelHeight / 2 - textHeight / 2);
			}
		}
	}

	// 10X10 Grid
	int numberOfLines = (rSettings.mGridSize / 10);
	if (rSettings.GetShow10X10Grid() == true)
	{
		wxPen* BoldLines = new wxPen(*wxBLACK, 2);
		dc.SetPen(*BoldLines);
		for (int i = 0; i < numberOfLines; i++)
		{
			wxPoint vStart(panelWidth * (i * 10), 0);
			wxPoint vEnd(panelWidth * (i * 10), this->GetSize().y);

			dc.DrawLine(vStart, vEnd);
		}

		for (int i = 0; i < numberOfLines; i++)
		{
			wxPoint hStart(0, panelHeight * (i * 10));
			wxPoint hEnd(this->GetSize().x ,panelHeight * (i * 10));

			dc.DrawLine(hStart, hEnd);
		}
	}
	// HUD
	if (rSettings.GetShowHUDOnOrOff() == true)
	{
		std::string boundryType;
		if (rSettings.GetIsFinite() == true)
		{
			boundryType = "Finite";
		}
		else
		{
			boundryType = "Toroidal";
		}
		pDrawingSurface->SetFont(wxFontInfo(16), *wxRED);
		wxString HUD;
		HUD = "Generation: " + std::to_string(mGeneration) + " \n" + "Living Cells: " + std::to_string(mLivingCells) + "\n" + "Boundry is: " + boundryType + "\n" + "Size of Universe: " + std::to_string(rSettings.mGridSize);
		double HUDTextWidth, HUDTextHeight;
		pDrawingSurface->GetTextExtent(HUD, &HUDTextWidth, &HUDTextHeight);
		wxDouble x = (double)this->GetSize().y - HUDTextHeight;
		pDrawingSurface->DrawText(HUD, 0, (double)this->GetSize().y - HUDTextHeight);
	}
}


void DrawingPanel::SetSize(wxSize size)
{
	if (this == nullptr) { return; }
	wxPanel::SetSize(size);
	Refresh();
	
}

void DrawingPanel::UpdateLivingCells()
{
	int alive = 0;
	for (int i = 0; i < rGameBoard.size(); i++)
	{
		for (int j = 0;j < rGameBoard[i].size(); j++)
		{
			if (rGameBoard[i][j] == true) { alive++; }

		}
	}
	SetCellCount(alive);
}

void DrawingPanel::SetGenCount(int gens)
{
	mGeneration = gens;
}


void DrawingPanel::MouseClick(wxMouseEvent& click)
{
	int x = click.GetX();
	int y = click.GetY();
	float cellWidth = GetSize().x / (float)rSettings.mGridSize;
	float cellHeight = GetSize().y / (float)rSettings.mGridSize;
	int column = x / cellWidth;
	int row = y / cellHeight;
	rGameBoard[column][row] = !rGameBoard[column][row];
	mParent->Refresh();
	UpdateLivingCells();
}

bool DrawingPanel::CheckCell(int column, int row)
{
	bool cellStatus = false;
	for (int i = 0; i < rGameBoard.size(); i++)
	{
		for (int j = 0; j < rGameBoard[i].size(); j++)
		{
			if (i == column && j == row)
			{
				bool cell = rGameBoard[i][j];
				if (cell == true)
				{
					cellStatus = true;
				}
				break;
			}
		}
	}
	return cellStatus;
}

int DrawingPanel::GetCellCount()
{
	return mLivingCells;
}

void DrawingPanel::SetCellCount(int cells)
{
	mLivingCells = cells;
}

int DrawingPanel::GetGenCount()
{
	return mGeneration;
}

void DrawingPanel::IncreaseGenCount(int gen)
{
	mGeneration = mGeneration + gen;
}

