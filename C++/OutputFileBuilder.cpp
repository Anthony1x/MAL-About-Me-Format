#include "OutputFileBuilder.h"
#include <numeric>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <format>

OutputFileBuilder::OutputFileBuilder(std::vector<VisualNovel>& visualNovels) : visualNovels(visualNovels)
{
}

std::string OutputFileBuilder::Build()
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

std::string OutputFileBuilder::BuildHeader()
{
	std::string output = "Since MyAnimeList does not support visual novels I will score them here";
	output += this->Break();
	output += this->Date();
	output += this->Break();

	return output;
}

std::string OutputFileBuilder::BuildFinishedVNs()
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
			output += this->TableData(visualNovel.getRating());
			output += this->TableData(visualNovel.getPlaytime());
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

size_t OutputFileBuilder::GetFinished()
{
	return std::count_if(visualNovels.begin(), visualNovels.end(),
		[](const VisualNovel& vn)
		{
			return vn.status == VisualNovel::Status::Finished;
		});
}

float OutputFileBuilder::MeanRating()
{
	float totalRating = std::accumulate(visualNovels.begin(), visualNovels.end(), 0.0,
		[](float sum, const VisualNovel& vn)
		{
			return vn.status == VisualNovel::Status::Finished ? sum + vn.rating : sum;
		});

	return totalRating / this->GetFinished();
}

int OutputFileBuilder::TotalHoursPlayed()
{
	return std::accumulate(visualNovels.begin(), visualNovels.end(), 0,
		[](int sum, const VisualNovel& vn)
		{
			return vn.status == VisualNovel::Status::Finished ? sum + vn.playtime : sum;
		});
}

std::string OutputFileBuilder::BuildCurrentlyReadingVNs()
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

std::string OutputFileBuilder::BuildOnHoldVNs()
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

std::string OutputFileBuilder::BuildPlanToReadVNs()
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

std::string OutputFileBuilder::BuildPlanToReadButCannotVNs()
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

std::string OutputFileBuilder::Break()
{
	return "\n";
}

std::string OutputFileBuilder::Date()
{
	std::time_t t = std::time(nullptr);
	std::tm now;

#ifdef _WIN32
	// For MSVC
	localtime_s(&now, &t);
#else
	// For POSIX
	localtime_r(&t, &now);
#endif

	std::stringstream ss;
	ss << "Last updated (DD/MM/YYYY): ";
	ss << std::setfill('0') << std::setw(2) << now.tm_mday << "/";
	ss << std::setfill('0') << std::setw(2) << now.tm_mon + 1 << "/";
	ss << now.tm_year + 1900;

	return ss.str();
}

std::string OutputFileBuilder::StartTable()
{
	return "[table]";
}

std::string OutputFileBuilder::StartTableRow()
{
	return "[tr]";
}

std::string OutputFileBuilder::EndTableRow()
{
	return "[/tr]";
}

std::string OutputFileBuilder::TableHeader(std::string value)
{
	return "[th]" + value + "[/th]";
}

std::string OutputFileBuilder::TableData(std::string value)
{
	return "[td]" + value + "[/td]";
}

std::string OutputFileBuilder::EndTable()
{
	return "[/table]";
}
