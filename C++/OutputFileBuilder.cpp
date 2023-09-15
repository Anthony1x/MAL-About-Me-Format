#include "OutputFileBuilder.h"
#include <numeric>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <format>

#ifdef _WIN32
// For MSVC
#define LOCALTIME(now, t) localtime_s(now, t);
#else
// For POSIX
#define LOCALTIME(now, t) localtime_r(t, now);
#endif

OutputFileBuilder::OutputFileBuilder(std::vector<VisualNovel>& visualNovels) : visualNovels(visualNovels)
{
}

const std::string OutputFileBuilder::Build() const
{
	std::string output = this->BuildHeader();

	output += this->BuildFinishedVNs();

	output += this->Break();

	output += this->StartTable();

	output += this->BuildCurrentlyReadingVNs();
	output += this->BuildOnHoldVNs();
	output += this->BuildPlanToReadVNs();
	output += this->BuildPlanToReadButCannotVNs();

	output += this->EndTable();

	return output;
}

const std::string OutputFileBuilder::BuildHeader() const
{
	std::string output = "Since MyAnimeList does not support visual novels I will score them here";
	output += this->Break();
	output += this->Date();
	output += this->Break();

	return output;
}

const std::string OutputFileBuilder::BuildFinishedVNs() const
{
	std::string output = this->StartTable();

	output += this->StartTableRow();

	output += this->TableHeader("Visual Novel");
	output += this->TableHeader("Rating");
	output += this->TableHeader("Play time");
	output += this->TableHeader("Comments");

	output += this->EndTableRow();

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Finished)
		{
			output += this->StartTableRow();

			output += this->TableData(visualNovel.name);
			output += this->TableData(visualNovel.GetRating());
			output += this->TableData(visualNovel.GetPlaytime());
			output += this->TableData(visualNovel.comment);

			output += this->EndTableRow();
		}
	}

	output += this->StartTableRow();

	output += this->TableHeader("GAMES: " + std::to_string(this->GetFinished()));
	output += this->TableHeader(std::format("{:.2f}", this->MeanRating()).append("/10"));
	output += this->TableHeader(std::to_string(this->TotalHoursPlayed()).append(" hours"));
	output += this->TableHeader("None");

	output += this->EndTableRow();

	output += this->EndTable();

	return output;
}

const std::string OutputFileBuilder::BuildCurrentlyReadingVNs() const
{
	std::string output = this->StartTableRow();
	output += this->TableHeader("Currently reading");
	output += this->EndTableRow();

	output += this->StartTableRow();

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Currently_Reading)
		{
			output += this->StartTableRow();
			output += this->TableData(visualNovel.name);
			output += this->EndTableRow();
		}
	}

	output += this->EndTableRow();

	return output;
}

const std::string OutputFileBuilder::BuildOnHoldVNs() const
{

	std::string output = this->StartTableRow();
	output += this->TableHeader("On hold");
	output += this->EndTableRow();

	output += this->StartTableRow();

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::On_Hold)
		{
			output += this->StartTableRow();
			output += this->TableData(visualNovel.name);
			output += this->EndTableRow();
		}
	}

	output += this->EndTableRow();

	return output;
}

const std::string OutputFileBuilder::BuildPlanToReadVNs() const
{
	std::string output = this->StartTableRow();
	output += this->TableHeader("Plan to read");
	output += this->EndTableRow();

	output += this->StartTableRow();

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Plan_To_Read)
		{
			output += this->StartTableRow();
			output += this->TableData(visualNovel.name);
			output += this->EndTableRow();
		}
	}

	output += this->EndTableRow();

	return output;
}

const std::string OutputFileBuilder::BuildPlanToReadButCannotVNs() const
{
	std::string output = this->StartTableRow();
	output += this->TableHeader("Plan to read, but unreleased / unlocalized");
	output += this->TableHeader("Comment");
	output += this->EndTableRow();

	output += this->StartTableRow();

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Plan_To_Read_But_Cannot)
		{
			output += this->StartTableRow();
			output += this->TableData(visualNovel.name);
			output += this->TableData(visualNovel.comment);
			output += this->EndTableRow();
		}
	}

	output += this->EndTableRow();

	return output;
}

const size_t OutputFileBuilder::GetFinished() const
{
	return std::count_if(visualNovels.begin(), visualNovels.end(),
		[](const VisualNovel& vn)
		{
			return vn.status == VisualNovel::Status::Finished;
		});
}

const float OutputFileBuilder::MeanRating() const
{
	float totalRating = std::accumulate(visualNovels.begin(), visualNovels.end(), 0.0,
		[](float sum, const VisualNovel& vn)
		{
			return vn.status == VisualNovel::Status::Finished ? sum + vn.rating : sum;
		});

	return totalRating / this->GetFinished();
}

const int OutputFileBuilder::TotalHoursPlayed() const
{
	return std::accumulate(visualNovels.begin(), visualNovels.end(), 0,
		[](int sum, const VisualNovel& vn)
		{
			return vn.status == VisualNovel::Status::Finished ? sum + vn.playtime : sum;
		});
}

const std::string OutputFileBuilder::Date() const
{
	std::time_t t = std::time(nullptr);
	std::tm now;

	LOCALTIME(&now, &t);

	std::stringstream ss;
	ss << "Last updated (DD/MM/YYYY): ";
	ss << std::setfill('0') << std::setw(2) << now.tm_mday << "/";
	ss << std::setfill('0') << std::setw(2) << now.tm_mon + 1 << "/";
	ss << now.tm_year + 1900;

	return ss.str();
}

const std::string OutputFileBuilder::Break() const
{
	return "\n";
}

const std::string OutputFileBuilder::StartTable() const
{
	return "[table]";
}

const std::string OutputFileBuilder::StartTableRow() const
{
	return "[tr]";
}

const std::string OutputFileBuilder::EndTableRow() const
{
	return "[/tr]";
}

const std::string OutputFileBuilder::TableHeader(std::string value) const
{
	return "[th]" + value + "[/th]";
}

const std::string OutputFileBuilder::TableData(std::string value) const
{
	return "[td]" + value + "[/td]";
}

const std::string OutputFileBuilder::EndTable() const
{
	return "[/table]";
}
