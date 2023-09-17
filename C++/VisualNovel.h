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
	VisualNovel(std::string name, uint8_t rating, uint8_t playtime, std::string comment);
	VisualNovel(std::string name, Status status);
	VisualNovel(std::string name, std::string comment);

	const wxString GetName() const;
	const std::string GetRating() const;
	const std::string GetPlaytime() const;
	const wxString GetComment() const;
};
