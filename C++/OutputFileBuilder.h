#pragma once
#include <string>
#include <vector>
#include "VisualNovel.h"

class OutputFileBuilder
{
public:
	OutputFileBuilder(std::vector<VisualNovel>& visualNovels);
	const std::string Build() const;

private:
	std::vector<VisualNovel>& visualNovels;

	const std::string BuildHeader() const;
	const std::string BuildCurrentlyReadingVNs() const;
	const std::string BuildOnHoldVNs() const;
	const std::string BuildPlanToReadVNs() const;
	const std::string BuildPlanToReadButCannotVNs() const;
	const std::string BuildFinishedVNs() const;

	const std::string Break() const;
	const std::string Date() const;
	const std::string StartTable() const;
	const std::string EndTable() const;
	const std::string StartTableRow() const;
	const std::string EndTableRow() const;
	const std::string TableHeader(std::string value) const;
	const std::string TableData(std::string value) const;

	const size_t GetFinished() const;
	const float MeanRating() const;
	const int TotalHoursPlayed() const;
};
