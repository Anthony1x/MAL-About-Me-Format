#pragma once
#include <string>
#include <vector>
#include "VisualNovel.h"

class OutputFileBuilder
{
public:
	OutputFileBuilder(std::vector<VisualNovel>& visualNovels);
	std::string Build();

private:
	std::string Break();

	std::string Date();

	std::string StartTable();
	std::string EndTable();

	std::string StartTableRow();
	std::string EndTableRow();

	std::string TableHeader(std::string value);

	std::string TableData(std::string value);

	std::vector<VisualNovel>& visualNovels;

	std::string BuildHeader();
	std::string BuildCurrentlyReadingVNs();
	std::string BuildOnHoldVNs();
	std::string BuildPlanToReadVNs();
	std::string BuildPlanToReadButCannotVNs();
	std::string BuildFinishedVNs();

	size_t getFinished();
	float meanRating();
	int totalHoursPlayed();
};
