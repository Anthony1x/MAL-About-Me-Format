#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/dataview.h>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	wxDataViewListCtrl* listCtrl;
	void OpenFileDialog(wxCommandEvent& evt);
	void SaveFileDialog(wxCommandEvent& evt);
	void FakePopulate();
};

