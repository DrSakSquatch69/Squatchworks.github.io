#include "SettingsDialog.h"
#include "MainWindow.h"

wxBEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
	EVT_BUTTON(wxID_OK, SettingsDialog::OKButton)
	EVT_BUTTON(wxID_CANCEL, SettingsDialog::CancelButton)
wxEND_EVENT_TABLE()

SettingsDialog::~SettingsDialog()
{
}

SettingsDialog::SettingsDialog(wxWindow* parent) :
	wxDialog(parent, wxID_ANY, "Settings", wxPoint(100, 100), wxSize(300, 300)), rSettings(((MainWindow*)parent)->mSettings)
{
	wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(pSizer);
	
	wxBoxSizer* pGridSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* pIntervalSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* pLivingColorSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* pDeadColorSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* pGridLineColor = new wxBoxSizer(wxHORIZONTAL);

	pGridSizeSelector = new wxSpinCtrl(this, 78321);
	pGridSizeSelector->SetValue(rSettings.mGridSize);
	wxStaticText* GridLabel = new wxStaticText(this, wxID_ANY, "Grid Size: ");
	pIntervalSelector = new wxSpinCtrl(this, 78322);
	pIntervalSelector->SetValue(rSettings.mInterval);
	wxStaticText* IntervalLabel = new wxStaticText(this, wxID_ANY, "Interval Per Mili-Second: ");
	pLivingCellColorSelector = new wxColourPickerCtrl(this, 10321);
	pLivingCellColorSelector->SetColour(rSettings.GetLivingColor());
	wxStaticText* LivingColorLabel = new wxStaticText(this, wxID_ANY, "Living Cell Color: ");
	PDeadCellColorSelector = new wxColourPickerCtrl(this, 10322);
	PDeadCellColorSelector->SetColour(rSettings.GetDeadColor());
	wxStaticText* DeadColorLabel = new wxStaticText(this, wxID_ANY, "Dead Cell Color: ");
	pGridLineColorSelector = new wxColourPickerCtrl(this, 10322);
	pGridLineColorSelector->SetColour(rSettings.GetGridColor());
	wxStaticText* GridColorLabel = new wxStaticText(this, wxID_ANY, "Grid Line Color: ");


	wxSizer* pOKandCancel = CreateButtonSizer(wxOK | wxCANCEL );
	pGridSizer->Add(GridLabel);
	pGridSizer->Add(pGridSizeSelector);
	pSizer->Add(pGridSizer);
	pIntervalSizer->Add(IntervalLabel);
	pIntervalSizer->Add(pIntervalSelector);
	pSizer->Add(pIntervalSizer);
	pLivingColorSizer->Add(LivingColorLabel);
	pLivingColorSizer->Add(pLivingCellColorSelector);
	pSizer->Add(pLivingColorSizer);
	pDeadColorSizer->Add(DeadColorLabel);
	pDeadColorSizer->Add(PDeadCellColorSelector);
	pGridLineColor->Add(GridColorLabel);
	pGridLineColor->Add(pGridLineColorSelector);
	pSizer->Add(pGridLineColor);
	pSizer->Add(pDeadColorSizer);
	pSizer->Add(pOKandCancel);
}

void SettingsDialog::OKButton(wxCommandEvent& ClickedOK)
{
	rSettings.mGridSize = pGridSizeSelector->GetValue();
	rSettings.mInterval = pIntervalSelector->GetValue();
	rSettings.SetLivingColor(pLivingCellColorSelector->GetColour());
	rSettings.SetDeadColor(PDeadCellColorSelector->GetColour());
	rSettings.SetGridColor(pGridLineColorSelector->GetColour());
	EndModal(wxID_OK);
}

void SettingsDialog::CancelButton(wxCommandEvent& ClickedCancel)
{
	EndModal(wxID_CANCEL);
}


