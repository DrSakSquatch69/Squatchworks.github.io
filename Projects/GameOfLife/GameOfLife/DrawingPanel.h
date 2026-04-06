#pragma once
#include "wx/wx.h"
#include "Settings.h"


class DrawingPanel :
    public wxPanel
{
private:
    wxFrame* mParent;
    std::vector<std::vector<int>>& rNeighborsCount;
    std::vector<std::vector<bool>>& rGameBoard;
    Settings& rSettings;
    int mLivingCells = 0;
    int mGeneration = 0;

    void OnPaint(wxPaintEvent& rEvent);
    void MouseClick(wxMouseEvent& click);

public:
    DrawingPanel(wxFrame* parent, wxSize size, std::vector<std::vector<bool>>& board, std::vector<std::vector<int>>& NeighborsCount, Settings& iSettings);
    ~DrawingPanel();

    bool CheckCell(int column, int row);
    int GetCellCount();
    void SetCellCount(int cells);
    int GetGenCount();
    void IncreaseGenCount(int gen);
    void SetSize(wxSize size);
    void UpdateLivingCells();
    void SetGenCount(int gens);
    
    wxDECLARE_EVENT_TABLE();
};
    

