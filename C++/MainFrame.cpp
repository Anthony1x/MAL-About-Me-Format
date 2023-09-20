#include "MainFrame.h"
#include <string>
#include <format>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <memory>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	wxPanel* panel = new wxPanel(this);

	wxMenuBar* menuBar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu();
	wxMenu* editMenu = new wxMenu();

	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(editMenu, _("&Edit"));

	// ---------- MENU BINDINGS ----------

	this->Bind(wxEVT_MENU, &MainFrame::OpenFileDialog, this, fileMenu->Append(wxID_OPEN)->GetId());
	// TODO: Don't open dialog. Make member that contains file path.
	this->Bind(wxEVT_MENU, &MainFrame::SaveFileDialog, this, fileMenu->Append(wxID_SAVE)->GetId());
	this->Bind(wxEVT_MENU, &MainFrame::SaveFileDialog, this, fileMenu->Append(wxID_SAVEAS)->GetId());
	this->Bind(wxEVT_MENU, [this](wxCommandEvent& evt) { Close(true); }, fileMenu->Append(wxID_EXIT)->GetId());

	this->Bind(wxEVT_MENU, [this](wxCommandEvent& evt) {
			uint32_t lastItem = listCtrlFinished->GetItemCount();

			wxVector<wxVariant> defaultValues;
			defaultValues.push_back("Title");
			defaultValues.push_back("Rating");
			defaultValues.push_back("Hours played");
			defaultValues.push_back("Comment");

			this->listCtrlFinished->AppendItem(defaultValues);

			// Get wxDataViewItem for the last row
			wxDataViewItem item = this->listCtrlFinished->RowToItem(lastItem);

			// Set the current item to the first cell of the last row
			this->listCtrlFinished->SetCurrentItem(item);
			this->listCtrlFinished->EditItem(item, this->listCtrlFinished->GetColumn(0));
		},
		editMenu->Append(wxID_ADD)->GetId());

	// Delete selected item
	this->Bind(wxEVT_MENU, [this](wxCommandEvent& evt) {
			uint32_t selectedItem = listCtrlFinished->GetSelectedRow();

			listCtrlFinished->DeleteItem(selectedItem);
		}, editMenu->Append(wxID_REMOVE)->GetId());

	// ---------- NOTEBOOK ----------

	wxNotebook* noteBook = new wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_TOP);

	noteBook->AddPage(CreateFinishedVisualNovelsPanel(noteBook), "Finished", true);
	noteBook->AddPage(CreateOnHoldVisualNovelsPanel(noteBook), "On Hold", false);
	noteBook->AddPage(CreateDroppedVisualNovelsPanel(noteBook), "Dropped", false);
	noteBook->AddPage(CreatePlanToReadVisualNovelsPanel(noteBook), "Plan to read", false);
	noteBook->AddPage(CreateCannotReadVisualNovelsPanel(noteBook), "Plan to read, but cannot", false);

	auto sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(noteBook, 1, wxEXPAND | wxALL, 10);
	panel->SetSizerAndFit(sizer);

	// --------- END ---------

	SetMenuBar(menuBar);

	wxStatusBar* statusBar = CreateStatusBar();
	statusBar->SetDoubleBuffered(true);
}

void MainFrame::OpenFileDialog(wxCommandEvent& evt)
{
	wxFileDialog openFileDialog(this, _("Open file"), wxEmptyString, wxEmptyString, "Visual Novel Data (*.vnd)|*.vnd", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL) return;

	wxFileInputStream input_stream(openFileDialog.GetPath());

	if (!input_stream.IsOk())
	{
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}

	wxTextInputStream text_stream(input_stream);
	std::string line;

	visualNovels.clear();

	VisualNovel::Status status = VisualNovel::Finished;

	uint8_t vnStatus = 0;

	while (!input_stream.Eof())
	{
		line = text_stream.ReadLine();

		// Line is empty, this means current Visual::Status has been populated.
		if (line.empty())
		{
			switch (vnStatus)
			{
			case VisualNovel::Finished:
				Populate(listCtrlFinished);
				break;
			case VisualNovel::On_Hold:
				Populate(listCtrlOnHold);
				break;
			case VisualNovel::Dropped:
				Populate(listCtrlDropped);
				break;
			case VisualNovel::Plan_To_Read:
				Populate(listCtrlPlanToRead);
				break;
			case VisualNovel::Plan_To_Read_But_Cannot:
				Populate(listCtrlPlanToReadButCannot);
				break;
			}
			vnStatus++;
			this->visualNovels.clear();
			
			continue;
		}

		wxArrayString components = wxStringTokenize(line, "\t");

		std::vector<std::string> values(components.begin(), components.end());

		/*
		 * FUNNY: I swear this is a feature, not a bug.
		 * VisualNovel objects solely accept `uint32_t`s for construction of hours played and scoring.
		 * Unfortunately, what save into the file is an entire string with non-number characters.
		 * We don't pass the value "34" as a string, we pass "34 hours".
		 * Similarly, we don't pass "7", we pass "7/10" for the rating.
		 * Fortunately, C++ (or maybe rather MSVC?), being quirky as it is,
		 * recognizes that the input has non-number characters and simply DISCARDS them.
		 * So, without having to do anything on our end, "34 hours" is converted to just 34, as an int.
		 * Take that, sensible developers!
		 * ...will have to look into how other compilers handle this.
		 */

		VisualNovel visualNovel = { };

		std::string name;
		uint8_t rating;
		uint8_t playtime;
		std::string comment;

		switch (vnStatus)
		{
		case VisualNovel::Finished:
			name = values[0];
			rating = std::stoi(values[1]);
			playtime = std::stoi(values[2]);
			comment = values[3];

			visualNovel = VisualNovel(name, rating, playtime, comment);
			break;

		case VisualNovel::On_Hold:
		case VisualNovel::Dropped:
		case VisualNovel::Plan_To_Read:
			name = values[0];

			visualNovel = VisualNovel(name, static_cast<VisualNovel::Status>(vnStatus));
			break;

		case VisualNovel::Plan_To_Read_But_Cannot:
			name = values[0];
			comment = values[1];

			visualNovel = VisualNovel(name, comment);
			break;
		}

		this->visualNovels.push_back(visualNovel);
	}
}

void MainFrame::SaveFileDialog(wxCommandEvent& evt)
{
	wxFileDialog saveFileDialog(this, _("Save file"), wxEmptyString, wxEmptyString, "Visual Novel Data (*.vnd)|*.vnd", wxFD_SAVE);

	if (saveFileDialog.ShowModal() == wxID_CANCEL) return;

	wxFileOutputStream output_stream(saveFileDialog.GetPath());

	if (!output_stream.IsOk())
	{
		wxLogError("Cannot save current contents in file '%s'.", saveFileDialog.GetPath());
		return;
	}

	VNWriteToFile(listCtrlFinished,				output_stream);
	VNWriteToFile(listCtrlOnHold,				output_stream);
	VNWriteToFile(listCtrlDropped,				output_stream);
	VNWriteToFile(listCtrlPlanToRead,			output_stream);
	VNWriteToFile(listCtrlPlanToReadButCannot,	output_stream);
}

void MainFrame::VNWriteToFile(wxDataViewListCtrl* listCtrl, wxFileOutputStream& output_stream)
{
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
				rowData += "\t";
		}

		rowData += "\n";

		// Convert rowData to a wxCharBuffer so it can be written to the file
		wxCharBuffer buffer = rowData.ToUTF8();

		// Write the row data to the file
		output_stream.Write(buffer, strlen(buffer));
	}
	if (listCtrl != this->listCtrlPlanToReadButCannot)
		output_stream.Write("\n", strlen("\n"));
}

wxPanel* MainFrame::CreateFinishedVisualNovelsPanel(wxNotebook* noteBook)
{
	wxPanel* pagePanel = new wxPanel(noteBook, wxID_ANY);
	listCtrlFinished = new wxDataViewListCtrl(pagePanel, wxID_ANY);

	listCtrlFinished->AppendTextColumn("Visual Novel", wxDATAVIEW_CELL_EDITABLE);
	listCtrlFinished->AppendTextColumn("Rating", wxDATAVIEW_CELL_EDITABLE);
	listCtrlFinished->AppendTextColumn("Play time", wxDATAVIEW_CELL_EDITABLE);
	listCtrlFinished->AppendTextColumn("Comment", wxDATAVIEW_CELL_EDITABLE);

	wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
	pageSizer->Add(listCtrlFinished, 1, wxALL | wxEXPAND, 0);
	pagePanel->SetSizerAndFit(pageSizer);

	return pagePanel;
}

wxPanel* MainFrame::CreateOnHoldVisualNovelsPanel(wxNotebook* noteBook)
{
	wxPanel* pagePanel = new wxPanel(noteBook, wxID_ANY);
	listCtrlOnHold = new wxDataViewListCtrl(pagePanel, wxID_ANY);

	listCtrlOnHold->AppendTextColumn("Visual Novel", wxDATAVIEW_CELL_EDITABLE);

	wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
	pageSizer->Add(listCtrlOnHold, 1, wxALL | wxEXPAND, 0);
	pagePanel->SetSizerAndFit(pageSizer);

	return pagePanel;
}

wxPanel* MainFrame::CreateDroppedVisualNovelsPanel(wxNotebook* noteBook)
{
	wxPanel* pagePanel = new wxPanel(noteBook, wxID_ANY);
	listCtrlDropped = new wxDataViewListCtrl(pagePanel, wxID_ANY);

	listCtrlDropped->AppendTextColumn("Visual Novel", wxDATAVIEW_CELL_EDITABLE);

	wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
	pageSizer->Add(listCtrlDropped, 1, wxALL | wxEXPAND, 0);
	pagePanel->SetSizerAndFit(pageSizer);

	return pagePanel;
}

wxPanel* MainFrame::CreatePlanToReadVisualNovelsPanel(wxNotebook* noteBook)
{
	wxPanel* pagePanel = new wxPanel(noteBook, wxID_ANY);
	listCtrlPlanToRead = new wxDataViewListCtrl(pagePanel, wxID_ANY);

	listCtrlPlanToRead->AppendTextColumn("Visual Novel", wxDATAVIEW_CELL_EDITABLE);

	wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
	pageSizer->Add(listCtrlPlanToRead, 1, wxALL | wxEXPAND, 0);
	pagePanel->SetSizerAndFit(pageSizer);

	return pagePanel;
}

wxPanel* MainFrame::CreateCannotReadVisualNovelsPanel(wxNotebook* noteBook)
{
	wxPanel* pagePanel = new wxPanel(noteBook, wxID_ANY);
	listCtrlPlanToReadButCannot = new wxDataViewListCtrl(pagePanel, wxID_ANY);

	listCtrlPlanToReadButCannot->AppendTextColumn("Visual Novel", wxDATAVIEW_CELL_EDITABLE);
	listCtrlPlanToReadButCannot->AppendTextColumn("Comment", wxDATAVIEW_CELL_EDITABLE);

	wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
	pageSizer->Add(listCtrlPlanToReadButCannot, 1, wxALL | wxEXPAND, 0);
	pagePanel->SetSizerAndFit(pageSizer);

	return pagePanel;
}

void MainFrame::Populate(wxDataViewListCtrl* listCtrl)
{
	if (visualNovels.empty()) return;

	listCtrl->DeleteAllItems();

	for (VisualNovel& visualNovel : visualNovels)
	{
		wxVector<wxVariant> defaultValues;

		if (listCtrl != this->listCtrlPlanToReadButCannot)
		{
			defaultValues.push_back(visualNovel.GetName());
			defaultValues.push_back(visualNovel.GetRating());
			defaultValues.push_back(visualNovel.GetPlaytime());
			defaultValues.push_back(visualNovel.GetComment());

			listCtrl->AppendItem(defaultValues);
			continue;
		}

		defaultValues.push_back(visualNovel.GetName());
		defaultValues.push_back(visualNovel.GetComment());

		listCtrl->AppendItem(defaultValues);
	}
}