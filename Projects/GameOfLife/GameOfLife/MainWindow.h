#pragma once
#include "wx/wx.h"
#include "DrawingPanel.h"
#include "Settings.h"
#include <vector>

class MainWindow :
    public wxFrame
{
private:
    DrawingPanel* pDrawingSurfacePointer = nullptr;
    wxBoxSizer* pSizer = nullptr;
    std::vector<std::vector<bool>> mGameBoard;
    wxStatusBar* pStatBar = nullptr;
    wxToolBar* pToolBar = nullptr;
    wxTimer* pTimer = nullptr;
    wxMenuBar* pMenuBar = nullptr;
    wxMenu* pOptionsMenu = nullptr;
    wxMenuItem* pRandoButton = nullptr;
    wxMenuItem* pRandoSeedButton = nullptr;
    wxMenu* pViewMenu = nullptr;
    wxMenuItem* pOptions = nullptr;
    wxMenuItem* pResetSettingsButton = nullptr;
    std::string savedFilePath = "";
    wxMenuItem* ToroidalItem = nullptr;
    wxMenuItem* FiniteItem = nullptr;
    wxMenuItem* pImport = nullptr;
    wxMenuItem* pShowGrid = nullptr;
    wxMenuItem* p10X10Grid = nullptr;
    wxMenuItem* pShowHUD = nullptr;

    void SizeChanged(wxSizeEvent& sizeChangeEvent);
    void ClickedPlay(wxCommandEvent& PlayButtonClicked);
    void ClickedPause(wxCommandEvent& PauseButtonClicked);
    void ClickedNext(wxCommandEvent& NextButtonClicked);
    void ClickedTrash(wxCommandEvent& TrashButtonClicked);
    void SettingsMenuClick(wxCommandEvent& SettingsMenuClicked);
    void Timer(wxTimerEvent& time);
    void CheckableClick(wxCommandEvent& SetCheckableClicked);
    void ClickedRandomize(wxCommandEvent& RandomizeClicked);
    void ClickedRandomizeWithSeed(wxCommandEvent& RandomizeWithSeedClicked);
    void SaveLogic();

public:
    std::vector<std::vector<int>> mNeighborCounts;
    Settings mSettings;
    MainWindow();
    ~MainWindow();

    void GameBoardInitialization();
    void StatBarUpdate();
    int CountLiveNeighbors(int row, int column);
    void IncrementGameBoardOneGeneration();
    void ClearButtonLogic();
    void RandomizeGrid(int sead);
    void RandomizeGridUserSeed(int seed);
    void OnNewClick(wxCommandEvent& NewClicked);
    void OnSaveClick(wxCommandEvent& SaveClicked);
    void OnOpenClick(wxCommandEvent& OpenClicked);
    void OnSaveAsClick(wxCommandEvent& SaveAsClicked);
    void OnExitClick(wxCommandEvent& ExitClicked);
    void OnUniverseClick(wxCommandEvent& TorodialClicked);
    void ResetSettingsClick(wxCommandEvent& ResetSettingsClicked);
    void ImportClick(wxCommandEvent& ImportClicked);
    void ShowGridClick(wxCommandEvent& ShowGridClicked);
    void Show10X10GridClick(wxCommandEvent& Show10X10GridClicked);
    void ShowHUDDisplay(wxCommandEvent& ShowHUDClicked);

    int FiniteCellCountMethod(int row, int column);
    int TorodialCellCountMethod(int row, int column);
    int CheckIndice(int x);
    wxDECLARE_EVENT_TABLE();
};

