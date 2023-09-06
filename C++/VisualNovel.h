#pragma once
#include <iostream>
#include <string>

struct VisualNovel
{
	enum Status
	{
		Finished = 0,
		Currently_Reading = 1,
		On_Hold = 2,
		Plan_To_Read = 3,
		Plan_To_Read_But_Cannot = 4
	};

	std::string name;
	uint8_t rating;
	uint8_t playtime;
	std::string comment;
	Status status;

	VisualNovel(std::string name, uint8_t rating, uint8_t playtime, std::string comment);
	VisualNovel(std::string name, Status status);
	VisualNovel(std::string name, std::string comment, Status status);

	std::string getRating();
	std::string getPlaytime();
};
