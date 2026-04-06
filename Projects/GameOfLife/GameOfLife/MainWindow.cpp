#include "MainWindow.h"
#include "play.xpm"
#include "pause.xpm"
#include "next.xpm"
#include "trash.xpm"
#include "wx/numdlg.h"
#include "wx/filedlg.h"
#include "SettingsDialog.h"

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_SIZE(MainWindow::SizeChanged)
	EVT_MENU(32199, MainWindow::ClickedPlay)
	EVT_MENU(32189, MainWindow::ClickedPause)
	EVT_MENU(32179, MainWindow::ClickedNext)
	EVT_MENU(32169, MainWindow::ClickedTrash)
	EVT_MENU(32159, MainWindow::SettingsMenuClick)
	EVT_MENU(32158, MainWindow::CheckableClick)
	EVT_MENU(32157, MainWindow::ClickedRandomize)
	EVT_MENU(32156, MainWindow::ClickedRandomizeWithSeed)
	EVT_MENU(wxID_SAVE, MainWindow::OnSaveClick)
	EVT_MENU(wxID_OPEN, MainWindow::OnOpenClick)
	EVT_MENU(wxID_NEW, MainWindow::OnNewClick)
	EVT_MENU(wxID_SAVEAS, MainWindow::OnSaveAsClick)
	EVT_MENU(wxID_EXIT, MainWindow::OnExitClick)
	EVT_MENU(32155, MainWindow::OnUniverseClick)
	EVT_MENU(32154, MainWindow::OnUniverseClick)
	EVT_MENU(32153, MainWindow::ResetSettingsClick)
	EVT_MENU(32152, MainWindow::ImportClick)
	EVT_MENU(32151, MainWindow::ShowGridClick)
	EVT_MENU(32150, MainWindow::Show10X10GridClick)
	EVT_MENU(32149, MainWindow::ShowHUDDisplay)
	EVT_TIMER(7814, MainWindow::Timer)
wxEND_EVENT_TABLE()


MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, "Game Of Life", wxPoint(0, 0), wxSize(200, 200))
{
	mSettings.LoadSettings();

	wxBitmap PlayIcon(play_xpm);
	wxBitmap PauseIcon(pause_xpm);
	wxBitmap NextIcon(next_xpm);
	wxBitmap TrashIcon(trash_xpm);
	
	pMenuBar = new wxMenuBar();
	SetMenuBar(pMenuBar);
	
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(wxID_NEW);
	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	fileMenu->Append(wxID_EXIT);
	pImport = new wxMenuItem(fileMenu, 32152, "Import", "", wxITEM_NORMAL);
	fileMenu->Append(pImport);
	pMenuBar->Append(fileMenu, "File");

	pOptionsMenu = new wxMenu();
	pRandoButton = new wxMenuItem(pOptionsMenu, 32157, "Randomize", "", wxITEM_NORMAL);
	pRandoSeedButton = new wxMenuItem(pOptionsMenu, 32156, "Randomize with Seed", "", wxITEM_NORMAL);
	pResetSettingsButton = new wxMenuItem(pOptionsMenu, 32153, "Reset Settings", "", wxITEM_NORMAL);
	pOptionsMenu->Append(32159, "Settings");
	pMenuBar->Append(pOptionsMenu, "Game");
	pOptionsMenu->Append(pResetSettingsButton);
	pOptionsMenu->Append(pRandoButton);
	pOptionsMenu->Append(pRandoSeedButton);
	
	
	pViewMenu = new wxMenu();
	pOptions = new wxMenuItem(pViewMenu, 32158, "Show Neigbor Count", "", wxITEM_CHECK);
	pOptions->SetCheckable(TRUE);
	ToroidalItem = new wxMenuItem(pViewMenu, 32155, "Torodial", "", wxITEM_CHECK);
	ToroidalItem->SetCheckable(TRUE);
	FiniteItem = new wxMenuItem(pViewMenu, 32154, "Finite", "", wxITEM_CHECK);
	FiniteItem->SetCheckable(TRUE);
	pShowGrid = new wxMenuItem(pViewMenu, 32151, "ShowGrid", "", wxITEM_CHECK);
	pShowGrid->SetCheckable(TRUE);
	p10X10Grid = new wxMenuItem(pViewMenu, 32150, "Show10X10Grid", "", wxITEM_CHECK);
	p10X10Grid->SetCheckable(TRUE);
	pShowHUD = new wxMenuItem(pViewMenu, 32149, "Show HUD", "", wxITEM_CHECK);
	pShowHUD->SetCheckable(TRUE);
	pViewMenu->Append(pOptions);
	pViewMenu->Append(ToroidalItem);
	pViewMenu->Append(FiniteItem);
	pViewMenu->Append(pShowGrid);
	pViewMenu->Append(p10X10Grid);
	pViewMenu->Append(pShowHUD);
	pMenuBar->Append(pViewMenu, "View");

	pStatBar = CreateStatusBar(2);
	pToolBar = CreateToolBar();
	pToolBar->AddTool(32199, "Play", PlayIcon);
	pToolBar->AddTool(32189, "Pause", PauseIcon);
	pToolBar->AddTool(32179, "Next", NextIcon);
	pToolBar->AddTool(32169, "Trash", TrashIcon);
	pToolBar->Realize();

	pTimer = new wxTimer(this, 7814);

	pDrawingSurfacePointer = new DrawingPanel(this, wxSize(100, 100), mGameBoard, mNeighborCounts, mSettings);

	pSizer = new wxBoxSizer(wxVERTICAL);
	pSizer->Add(pDrawingSurfacePointer, 1, wxEXPAND | wxALL);
	SetSizer(pSizer);
	
	GameBoardInitialization();
	StatBarUpdate();
	this->Layout();
}

MainWindow::~MainWindow()
{

}

// Event handler
void MainWindow::SizeChanged(wxSizeEvent& sizeChangeEvent)
{
	if (!pDrawingSurfacePointer) { return; }
	pDrawingSurfacePointer->SetSize(sizeChangeEvent.GetSize());
	sizeChangeEvent.Skip();
}

void MainWindow::ClickedPlay(wxCommandEvent& PlayButtonClicked)
{
	pTimer->Start(mSettings.mInterval);
	PlayButtonClicked.Skip();
}

void MainWindow::ClickedPause(wxCommandEvent& PauseButtonClicked)
{
	pTimer->Stop();
	PauseButtonClicked.Skip();
}

void MainWindow::ClickedNext(wxCommandEvent& NextButtonClicked)
{
	this->IncrementGameBoardOneGeneration();
	NextButtonClicked.Skip();
}

void MainWindow::ClickedTrash(wxCommandEvent& TrashButtonClicked)
{
	this->ClearButtonLogic();
	TrashButtonClicked.Skip();
}

void MainWindow::SettingsMenuClick(wxCommandEvent& SettingsMenuClicked)
{
	SettingsDialog* SettingsMenu = new SettingsDialog(this);
	if (SettingsMenu->ShowModal() == wxID_OK)
	{
		GameBoardInitialization();
		pDrawingSurfacePointer->Refresh();
		mSettings.SaveSettings();
	}
	else
		mSettings.LoadSettings();
	SettingsMenuClicked.Skip();
}

void MainWindow::Timer(wxTimerEvent& time)
{
	this->IncrementGameBoardOneGeneration();
	time.Skip();
}

void MainWindow::CheckableClick(wxCommandEvent& SetCheckableClicked)
{
	if (mSettings.GetNeighborCountOnOrOff() == true)
		pOptions->Check(FALSE);
	else
		pOptions->Check(TRUE);
	
	mSettings.SetNeighborCountOnOrOff(pOptions->IsChecked());

	pDrawingSurfacePointer->Refresh();	
	mSettings.SaveSettings();
	SetCheckableClicked.Skip();
}

void MainWindow::ClickedRandomize(wxCommandEvent& RandomizeClicked)
{
	RandomizeGrid(time(NULL));
	RandomizeClicked.Skip();

}

void MainWindow::ClickedRandomizeWithSeed(wxCommandEvent& RandomizeWithSeedClicked)
{
	int userSeed = wxGetNumberFromUser("Seed to Use for Randomization", "Seed should be between 0 and 1000", "Enter Number: ", 50, 0, 1000, NULL);
	RandomizeGridUserSeed(userSeed);
	RandomizeWithSeedClicked.Skip();
}


void MainWindow::SaveLogic()
{
	std::ofstream fileStream;
	fileStream.open(savedFilePath);
	if (fileStream.is_open())
	{
		for (int i = 0; i < mGameBoard.size(); i++)
		{
			for (int j = 0; j < mGameBoard[i].size(); j++)
			{
				if (mGameBoard[i][j])
				{
					fileStream << '*';
				}
				else
				{
					fileStream << '.';
				}
			}
			fileStream << '\n';
		}
		fileStream.close();
	}
}

void MainWindow::GameBoardInitialization()
{
	mGameBoard.resize(mSettings.mGridSize);
	mNeighborCounts.resize(mSettings.mGridSize);
	for (int i = 0; i < mGameBoard.size(); i++)
	{
		mGameBoard[i].resize(mSettings.mGridSize);
		mNeighborCounts[i].resize(mSettings.mGridSize);
	}
}

void MainWindow::StatBarUpdate()
{
	int livingCells = pDrawingSurfacePointer->GetCellCount();
	int Generations = pDrawingSurfacePointer->GetGenCount();
	
	SetStatusText(wxString::Format("Living Cells: %d", livingCells), 0);
	SetStatusText(wxString::Format("Generations: %d", Generations), 1);	
}

int MainWindow::CountLiveNeighbors(int row, int column)
{
	int livingNeighbors = 0; // variable to store number of living cells
	
	if (mSettings.GetIsFinite() == true)
	{
		livingNeighbors = FiniteCellCountMethod(row, column);
	}
	else
	{
		livingNeighbors = TorodialCellCountMethod(row, column);
	}

	return livingNeighbors; // return result
}

void MainWindow::IncrementGameBoardOneGeneration()
{
	int livingCells = 0;
	int generation = 1;
	std::vector<std::vector<bool>> SandBox;
	SandBox.resize(mSettings.mGridSize);
	for (int i = 0; i < mSettings.mGridSize; i++)
		SandBox[i].resize(mSettings.mGridSize);

	for (int i = 0; i < mSettings.mGridSize; i++)
	{
		for (int j = 0; j < mSettings.mGridSize; j++)
		{
			int neighbors = CountLiveNeighbors(i, j);
			
			if (mGameBoard[i][j] == true) // if cell is alive
			{
				SandBox[i][j] = true; // turn sandbox cell alive

				if (neighbors < 2 || neighbors > 3) // rules if less than 2 living neihbors or more then 3
					SandBox[i][j] = false; // change to dead
				if (SandBox[i][j] == true) // if sandbox cell is still alive
					livingCells++; // increment living cell count
			}
			else //if dead
			{
				SandBox[i][j] = false; // sandbox cell dead
				if (neighbors == 3) // if 3 living neighbors
				{
					SandBox[i][j] = true; // sandbox cell comes alive
					livingCells++; // increment living cell count
				}
			}
		}
	}
	mGameBoard.swap(SandBox); // swapping gameboards;
	for (int i = 0; i < mSettings.mGridSize; i++)
	{
		for (int j = 0; j < mSettings.mGridSize; j++)
		{
			int cellText = CountLiveNeighbors(i, j);
			mNeighborCounts[i][j] = cellText;
		}
	}
	SandBox.clear();
	pDrawingSurfacePointer->SetCellCount(livingCells); // setting living cell Counter
	pDrawingSurfacePointer->IncreaseGenCount(generation); //incrementing generation counter
	StatBarUpdate(); // updating stat bar
	pDrawingSurfacePointer->Refresh(); // refreshing drawing panel
}

void MainWindow::ClearButtonLogic()
{
	for (int i = 0; i < mSettings.mGridSize; i++)
	{
		for (int j = 0; j < mSettings.mGridSize; j++)
		{
			mGameBoard[i][j] = false;
		}
	}
	pDrawingSurfacePointer->SetCellCount(0);
	pDrawingSurfacePointer->SetGenCount(0);
	StatBarUpdate();
	pDrawingSurfacePointer->Refresh();
}

void MainWindow::RandomizeGrid(int sead)
{
	srand(sead);
	for (int i = 0; i < mSettings.mGridSize; i++)
	{
		for (int j = 0; j < mSettings.mGridSize; j++)
		{
			int randomNumber = rand();
			int min = 15;
			int max = 75;
			int num = min + (rand() % (max - min + 1));
			if (randomNumber % 100 < num)
				mGameBoard[i][j] = true;
			else
				mGameBoard[i][j] = false;
		}
	}
	pDrawingSurfacePointer->Refresh();
}

void MainWindow::RandomizeGridUserSeed(int seed)
{
	srand(seed);
	for (int i = 0; i < mSettings.mGridSize; i++)
	{
		for (int j = 0; j < mSettings.mGridSize; j++)
		{
			int randomNumber = rand();
			if (randomNumber % 3 == 0)
				mGameBoard[i][j] = true;
			else
				mGameBoard[i][j] = false;
		}
	}
	pDrawingSurfacePointer->Refresh();
}

void MainWindow::OnNewClick(wxCommandEvent& NewClicked)
{
	this->ClearButtonLogic();
	savedFilePath = "";
	NewClicked.Skip();
}

void MainWindow::OnSaveClick(wxCommandEvent& SaveClicked)
{
	if (savedFilePath == "")
	{
		wxFileDialog fileDialog(this, "Save Game of Life File", wxEmptyString, wxEmptyString, "Game of Life Files (*.cells) | *.cells", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (fileDialog.ShowModal() == wxID_CANCEL)
		{
			return;
		}
		savedFilePath = (std::string)fileDialog.GetPath();
	}

	MainWindow::SaveLogic();
	SaveClicked.Skip();
}

void MainWindow::OnOpenClick(wxCommandEvent& OpenClicked)
{
	wxFileDialog fileDialog(this, "Open Game of Life File", wxEmptyString, wxEmptyString, "Game of Life Files (*.cells) | *.cells", wxFD_OPEN);
	if (fileDialog.ShowModal() == wxID_CANCEL)
		return;
	
	for (int i = 0; i < mGameBoard.size(); i++)
	{
		mGameBoard[i].clear();
		mGameBoard[i].resize(0);
	}
	mGameBoard.clear();
	mGameBoard.resize(0);

	std::string buffer;
	int index = 0;
	std::ifstream fileStream;
	fileStream.open((std::string)fileDialog.GetPath());
	if (fileStream.is_open())
	{
		while (!fileStream.eof())
		{
			std::getline(fileStream, buffer);
			if (buffer.size() == 0) { break; }
			if (mGameBoard.size() == 0)
			{
				mGameBoard.resize(buffer.size());
				mSettings.mGridSize = buffer.size();
			}
			mGameBoard[index].resize(buffer.size());
			for (int i = 0; i < buffer.size(); i++)
			{
				if (buffer[i] == '*')
				{
					mGameBoard[i][index] = true;
				}
				else
				{
					mGameBoard[i][index] = false;
				}
			}
			index++;
		}
		fileStream.close();
	}

	OpenClicked.Skip();
}

void MainWindow::OnSaveAsClick(wxCommandEvent& SaveAsClicked)
{
	wxFileDialog fileDialog(this, "Save Game of Life File", wxEmptyString, wxEmptyString, "Game of Life Files (*.cells) | *.cells", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	savedFilePath = (std::string)fileDialog.GetPath();
	std::ofstream fileStream;
	
	fileStream.open(savedFilePath);
	
	if (fileStream.is_open())
	{
		for (int i = 0; i < mGameBoard.size(); i++)
		{
			for (int j = 0; j < mGameBoard[i].size(); j++)
			{
				if (mGameBoard[i][j])
				{
					fileStream << '*';
				}
				else
				{
					fileStream << '.';
				}
			}
			fileStream << '\n';
		}
		fileStream.close();
	}
	SaveAsClicked.Skip();
}

void MainWindow::OnExitClick(wxCommandEvent& ExitClicked)
{
	this->Close();
}

void MainWindow::OnUniverseClick(wxCommandEvent& UniverseClicked)
{
	if (mSettings.GetIsFinite() == true)
	{
		ToroidalItem->Check();
		FiniteItem->Check(FALSE);
	}
	else
	{
		FiniteItem->Check();
		ToroidalItem->Check(FALSE);
	}
	mSettings.SetIsFinite(FiniteItem->IsChecked());
	pDrawingSurfacePointer->Refresh();
	mSettings.SaveSettings();
	UniverseClicked.Skip();
}

void MainWindow::ResetSettingsClick(wxCommandEvent& ResetSettingsClicked)
{
	mSettings.RefreshSettings();
	pDrawingSurfacePointer->Refresh();
	ResetSettingsClicked.Skip();
}

void MainWindow::ImportClick(wxCommandEvent& ImportClicked)
{
	wxFileDialog fileDialog(this, "Import Game of Life File", wxEmptyString, wxEmptyString, "Game of Life Files (*.cells) | *.cells", wxFD_OPEN);
	if (fileDialog.ShowModal() == wxID_CANCEL)
		return;

	std::string buffer;
	int index = 0;
	std::ifstream fileStream;
	fileStream.open((std::string)fileDialog.GetPath());
	if (fileStream.is_open())
	{
		while (!fileStream.eof())
		{
			std::getline(fileStream, buffer);
			if (buffer.size() == 0) { break; }
			for (int i = 0; i < buffer.size(); i++)
			{
				if (buffer[i] == '*')
				{
					mGameBoard[i][index] = true;
				}
				else
				{
					mGameBoard[i][index] = false;
				}
			}
			index++;
		}
		fileStream.close();
	}
	ImportClicked.Skip();
}

void MainWindow::ShowGridClick(wxCommandEvent& ShowGridClicked)
{
	if(mSettings.GetShowingGrid() == true)
	{
		pShowGrid->Check(FALSE);
	}
	else
	{
		pShowGrid->Check(TRUE);
	}
	mSettings.SetShowGrid(pShowGrid->IsChecked());
	pDrawingSurfacePointer->Refresh();
	mSettings.SaveSettings();
	ShowGridClicked.Skip();
}

void MainWindow::Show10X10GridClick(wxCommandEvent& Show10X10GridClicked)
{
	if (mSettings.mShowing10X10Grid == true)
	{
		p10X10Grid->Check(FALSE);
	}
	else
	{
		p10X10Grid->Check(TRUE);
	}
	mSettings.SetShow10X10Grid(p10X10Grid->IsChecked());
	pDrawingSurfacePointer->Refresh();
	mSettings.SaveSettings();
	Show10X10GridClicked.Skip();
}

void MainWindow::ShowHUDDisplay(wxCommandEvent& ShowHUDClicked)
{
	if (mSettings.GetShowHUDOnOrOff() == true)
	{
		pShowHUD->Check(FALSE);
	}
	else
	{
		pShowHUD->Check(TRUE);
	}
	mSettings.SetShowHUDOnOrOff(pShowHUD->IsChecked());
	pDrawingSurfacePointer->Refresh();
	mSettings.SaveSettings();
	ShowHUDClicked.Skip();
}

int MainWindow::FiniteCellCountMethod(int row, int column)
{
	int livingNeighbors = 0; // variable to store number of living cells
	for (int i = -1; i < 2; i++) //nested for loops
	{
		for (int j = -1; j < 2; j++)
		{
			int x = row + i;
			int y = column + j;

			if (x == -1 || y == -1) // if checked cell will be out of range
			{
				continue;
			}
			if (x >= mSettings.mGridSize || y >= mSettings.mGridSize) // out of range
			{
				continue;
			}
			if (i == 0 && j == 0) // if checked cell is same as original
			{
				continue;
			}
			if (mGameBoard[x][y] == true) // if neighboring cell is alive
			{
				livingNeighbors++; //increment variable
			}
		}
	}
	return livingNeighbors;
}

int MainWindow::TorodialCellCountMethod(int row, int column)
{
	int CellCount = 0;
	for (int i = -1; i < 2; i++) //nested for loops
	{
		for (int j = -1; j < 2; j++)
		{
			int x = row + i;
			int y = column + j;

			if (i == 0 && j == 0) // if checked cell is same as original
			{
				continue;
			}

			x = CheckIndice(x);
			y = CheckIndice(y);

			if (mGameBoard[x][y] == true)
			{
				CellCount++;
			}
		}
	}

	return CellCount;
}

int MainWindow::CheckIndice(int Indice)
{
	int returnNum;
	if (Indice == -1)
	{
		returnNum = (mSettings.mGridSize - 1);
	}
	else if (Indice == mSettings.mGridSize)
	{
		returnNum = 0;
	}
	else
	{
		returnNum = Indice;
	}
	return returnNum;
}
