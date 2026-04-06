#pragma once
#include "wx/wx.h"
#include "Settings.h"
#include "wx/spinctrl.h"
#include "wx/clrpicker.h"

class SettingsDialog :
    public wxDialog
{
private:

    Settings& rSettings;
    wxSpinCtrl* pGridSizeSelector = nullptr;
    wxSpinCtrl* pIntervalSelector = nullptr;
    wxColourPickerCtrl* pLivingCellColorSelector = nullptr;
    wxColourPickerCtrl* PDeadCellColorSelector = nullptr;
    wxColourPickerCtrl* pGridLineColorSelector = nullptr;

    void OKButton(wxCommandEvent& ClickedOK);
    void CancelButton(wxCommandEvent& ClickedCancel);


public:

    SettingsDialog(wxWindow* parent);
    ~SettingsDialog();


    wxDECLARE_EVENT_TABLE();
};

