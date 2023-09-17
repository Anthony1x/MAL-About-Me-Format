#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/dataview.h>
#include <wx/notebook.h>
#include <wx/wfstream.h>
#include "VisualNovel.h"

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	std::vector<VisualNovel> visualNovels = { };

	wxDataViewListCtrl* listCtrlFinished;
	wxDataViewListCtrl* listCtrlOnHold;
	wxDataViewListCtrl* listCtrlDropped;
	wxDataViewListCtrl* listCtrlPlanToRead;
	wxDataViewListCtrl* listCtrlPlanToReadButCannot;
	
	// When opening or saving a file, save the path in this variable for Ctrl+S.
	wxString filePath;
	
	void OpenFileDialog(wxCommandEvent& evt);
	void SaveFileDialog(wxCommandEvent& evt);
	void VNWriteToFile(wxDataViewListCtrl* listCtrl, wxFileOutputStream& output_stream);
	wxPanel* CreateFinishedVisualNovelsPanel(wxNotebook* noteBook);
	wxPanel* CreateOnHoldVisualNovelsPanel(wxNotebook* noteBook);
	wxPanel* CreateDroppedVisualNovelsPanel(wxNotebook* noteBook);
	wxPanel* CreatePlanToReadVisualNovelsPanel(wxNotebook* noteBook);
	wxPanel* CreateCannotReadVisualNovelsPanel(wxNotebook* noteBook);
	void Populate(wxDataViewListCtrl* listCtrl);
};

