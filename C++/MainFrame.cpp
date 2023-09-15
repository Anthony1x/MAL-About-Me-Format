#include "MainFrame.h"
#include <string>
#include <format>
#include <wx/editlbox.h>
#include <wx/wfstream.h>
#include "VisualNovel.h"

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	wxPanel* panel = new wxPanel(this);

	wxMenuBar* menuBar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu();
	wxMenu* editMenu = new wxMenu();

	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(editMenu, _("&Edit"));

	fileMenu->Append(wxID_OPEN);
	fileMenu->Append(wxID_SAVE);
	fileMenu->Append(wxID_SAVEAS);
	// TOOD: Bind exit to close the application.
	fileMenu->Append(wxID_EXIT);

	editMenu->Append(wxID_ADD);
	editMenu->Append(wxID_REMOVE);

	this->Bind(wxEVT_MENU, &MainFrame::OpenFileDialog, this, wxID_OPEN);
	this->Bind(wxEVT_MENU, &MainFrame::SaveFileDialog, this, wxID_SAVEAS);

	listCtrl = new wxDataViewListCtrl(panel, wxID_ANY);

	listCtrl->AppendTextColumn("Visual Novel", wxDATAVIEW_CELL_EDITABLE);
	listCtrl->AppendTextColumn("Rating", wxDATAVIEW_CELL_EDITABLE);
	listCtrl->AppendTextColumn("Play time", wxDATAVIEW_CELL_EDITABLE);
	listCtrl->AppendTextColumn("Comment", wxDATAVIEW_CELL_EDITABLE);

	auto sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(listCtrl, 1, wxALL|wxEXPAND, 0);
	panel->SetSizerAndFit(sizer);

	FakePopulate();

	// --------- END ---------

	SetMenuBar(menuBar);

	wxStatusBar* statusBar = CreateStatusBar();
	statusBar->SetDoubleBuffered(true);
}

void MainFrame::OpenFileDialog(wxCommandEvent& evt)
{
	wxFileDialog openFileDialog(this, _("Open a fucking file"), wxEmptyString, wxEmptyString, "Visual Novel Data (*.vnd)|*.vnd", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL) return;

	wxFileInputStream input_stream(openFileDialog.GetPath());

	if (!input_stream.IsOk())
	{
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}

	// TODO: Create function to read a stream and convert it to VN list.
}

void MainFrame::SaveFileDialog(wxCommandEvent& evt)
{
	wxFileDialog saveFileDialog(this, _("Save a fucking file"), wxEmptyString, wxEmptyString, "Visual Novel Data (*.vnd)|*.vnd", wxFD_SAVE);

	if (saveFileDialog.ShowModal() == wxID_CANCEL) return;

	wxFileOutputStream output_stream(saveFileDialog.GetPath());

	if (!output_stream.IsOk())
	{
		wxLogError("Cannot save current contents in file '%s'.", saveFileDialog.GetPath());
		return;
	}

	int numRows = listCtrl->GetItemCount();
	int numCols = listCtrl->GetColumnCount();

	for (int row = 0; row < numRows; ++row)
	{
		wxString rowData;

		for (int col = 0; col < numCols; ++col)
		{
			wxString cellValue = listCtrl->GetTextValue(row, col);
			rowData += cellValue;

			// Add a tab after each cell, except the last cell in the row
			if (col < numCols - 1)
			{
				rowData += "\t";
			}
		}

		// Add a newline character after each row
		rowData += "\n";

		// Convert rowData to a wxCharBuffer so it can be written to the file
		wxCharBuffer buffer = rowData.ToUTF8();

		// Write the row data to the file
		output_stream.Write(buffer, strlen(buffer));
	}
}

void MainFrame::FakePopulate()
{
	std::vector<VisualNovel> visualNovels = {
		VisualNovel("Steins;Gate",                              8,	34,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Steins;Gate 0",                            7,	32,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Chaos;Head",                               6,	28,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Robotics;Notes ELITE",                     8,	42,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Chaos;Child",                              9,	54,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Fate/Stay Night (Realta Nua)",             8,	78,	"PATCH USED: Ultimate Edition Patch"),
		VisualNovel("Fate/Hollow Ataraxia",                     7,	32,	"PATCH USED: Voice Patch"),
		VisualNovel("Wonderful Everyday Down the Rabbit-Hole",  10, 50, "None"),
		VisualNovel("Phoenix Wright: Ace Attorney Trilogy",     8,	61,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("eden*",                                    6,	9,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Chaos;Head Noah",                          8,	47,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("AI: The Somnium Files",                    6,	24,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("DeadΩAegis: Gaiden",						5,	2,	"Prequel to DeadΩAegis"),
	};

	for (VisualNovel& visualNovel : visualNovels)
	{
		wxVector<wxVariant> data;
		data.push_back(wxVariant(visualNovel.name));
		data.push_back(wxVariant(visualNovel.GetRating()));
		data.push_back(wxVariant(visualNovel.GetPlaytime()));
		data.push_back(wxVariant(visualNovel.comment));
		listCtrl->AppendItem(data);
	}
}