#pragma once
#include <iostream>
#include <string>
#include <wx/string.h>

struct VisualNovel
{
	enum Status
	{
		Finished,
		Currently_Reading,
		On_Hold,
		Dropped,
		Plan_To_Read,
		Plan_To_Read_But_Cannot
	};

	std::string name;
	uint8_t rating;
	uint8_t playtime;
	std::string comment;
	Status status;

	VisualNovel();
	
	// Finished
	VisualNovel(std::string name, uint8_t rating, uint8_t playtime, std::string comment);

	// Currently reading, On hold, Dropped, Plan to read
	VisualNovel(std::string name, Status status);

	// Plan to read, but can't
	VisualNovel(std::string name, std::string comment);

	const wxString GetName() const;
	const std::string GetRating() const;
	const std::string GetPlaytime() const;
	const wxString GetComment() const;
};
