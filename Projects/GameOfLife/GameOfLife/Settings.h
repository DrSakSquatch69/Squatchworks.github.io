#pragma once
#include "wx/wx.h"
#include <fstream>

struct Settings {
	int mGridSize = 25;
	int mInterval = 50;
	bool mShowNeighborCount = false;
	bool mIsFinite = true;
	bool mShowingGrid = true;
	bool mShowing10X10Grid = true;
	bool mShowHUD = false;
	
	unsigned int LRed = 128;
	unsigned int LGreen = 128;
	unsigned int LBlue = 128;
	unsigned int LAlpha = 255;

	unsigned int DRed = 255;
	unsigned int DGreen = 255;
	unsigned int DBlue = 255;
	unsigned int DAlpha = 255;

	unsigned int GRed = 0;
	unsigned int GGreen = 0;
	unsigned int GBlue = 0;
	unsigned int GAlpha = 65;

	void SetGridColor(wxColor GridColor)
	{
		GRed = GridColor.Red();
		GGreen = GridColor.Green();
		GBlue = GridColor.Blue();
		GAlpha = 65;
	}

	wxColour GetGridColor()
	{
		wxColor GridColor(GRed, GGreen, GBlue, GAlpha);
		return GridColor;
	}

	void SetLivingColor(wxColor LivingColor)
	{
		LRed = LivingColor.Red();
		LGreen = LivingColor.Green();
		LBlue = LivingColor.Blue();
		LAlpha = LivingColor.Alpha();
	}

	void SetDeadColor(wxColor DeadColor)
	{
		DRed = DeadColor.Red();
		DGreen = DeadColor.Green();
		DBlue = DeadColor.Blue();
		DAlpha = DeadColor.Alpha();
	}

	wxColour GetLivingColor()
	{
		wxColor LivingColor(LRed, LGreen, LBlue, LAlpha);
		return LivingColor;
	}

	wxColour GetDeadColor()
	{
		wxColor DeadColor(DRed, DGreen, DBlue, DAlpha);
		return DeadColor;
	}

	void SaveSettings()
	{
		std::ofstream file("settings.bin", std::ios::out | std::ios::binary);
		file.write((char*)this, sizeof(Settings));
		file.close();
	}

	void LoadSettings()
	{	
		//if check to see if there is bin file and if not what needs to be done!
		
		std::ifstream file("settings.bin", std::ios::binary | std::ios::in);
		if(file.good())
		{
			file.read((char*)this, sizeof(Settings));
			file.close();
		}
		else
		{
			SaveSettings();
			file.read((char*)this, sizeof(Settings));
			file.close();
		}
	}

	void SetShow10X10Grid(bool lines)
	{
		mShowing10X10Grid = lines;
	}
	bool GetShow10X10Grid()
	{
		return mShowing10X10Grid;
	}
	void SetShowGrid(bool Grid)
	{
		mShowingGrid = Grid;
	}
	bool GetShowingGrid()
	{
		return mShowingGrid;
	}

	void SetIsFinite(bool finite)
	{
		mIsFinite = finite;
	}
	bool GetIsFinite()
	{
		return mIsFinite;
	}

	void SetNeighborCountOnOrOff(bool count)
	{
		mShowNeighborCount = count;
	}
	bool GetNeighborCountOnOrOff()
	{
		return mShowNeighborCount;
	}

	void SetShowHUDOnOrOff(bool HUD)
	{
		mShowHUD = HUD;
	}
	bool GetShowHUDOnOrOff()
	{
		return mShowHUD;
	}

	void RefreshSettings()
	{
		mGridSize = 25;
		mInterval = 50;
		mShowNeighborCount = false;
		mIsFinite = true;
		mShowingGrid = true;
		mShowing10X10Grid = true;
		mShowHUD = false;

		LRed = 128;
		LGreen = 128;
		LBlue = 128;
		LAlpha = 255;

		DRed = 255;
		DGreen = 255;
		DBlue = 255;
		DAlpha = 255;
	
		GRed = 0;
		GGreen = 0;
		GBlue = 0;
		GAlpha = 65;
		
		SaveSettings();
		LoadSettings();
	}

};