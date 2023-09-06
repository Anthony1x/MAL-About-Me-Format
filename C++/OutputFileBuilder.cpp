#include "OutputFileBuilder.h"
#include <numeric>
#include <algorithm>
#include <format>

OutputFileBuilder::OutputFileBuilder(std::vector<VisualNovel>& visualNovels) : visualNovels(visualNovels)
{
}

std::string OutputFileBuilder::Build()
{
	std::string output = this->BuildHeader();

	output.append(this->BuildFinishedVNs());

	output.append(this->Break());

	output.append(this->StartTable());

	output.append(this->BuildCurrentlyReadingVNs());
	output.append(this->BuildOnHoldVNs());
	output.append(this->BuildPlanToReadVNs());
	output.append(this->BuildPlanToReadButCannotVNs());

	output.append(this->EndTable());

	return output;
}

std::string OutputFileBuilder::BuildHeader() {
	std::string output = "Since MyAnimeList does not support visual novels I will score them here";
	output.append(this->Break());
	output.append(this->Date());
	output.append(this->Break());

	return output;
}

std::string OutputFileBuilder::BuildFinishedVNs() {
	std::string output = this->StartTable();

	output.append(this->StartTableRow());

	output.append(this->TableHeader("Visual Novel"));
	output.append(this->TableHeader("Rating"));
	output.append(this->TableHeader("Play time"));
	output.append(this->TableHeader("Comments"));

	output.append(this->EndTableRow());

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Finished) {

			output.append(this->StartTableRow());

			output.append(this->TableData(visualNovel.name));
			output.append(this->TableData(visualNovel.getRating()));
			output.append(this->TableData(visualNovel.getPlaytime()));
			output.append(this->TableData(visualNovel.comment));

			output.append(this->EndTableRow());
		}
	}

	output.append(this->StartTableRow());

	output.append(this->TableHeader("GAMES: " + std::to_string(this->getFinished())));
	output.append(this->TableHeader(std::format("{:.2f}", this->meanRating()).append("/10")));
	output.append(this->TableHeader(std::to_string(this->totalHoursPlayed()).append(" hours")));
	output.append(this->TableHeader("None"));

	output.append(this->EndTableRow());


	output.append(this->EndTable());

	return output;
}

size_t OutputFileBuilder::getFinished()
{
	return std::count_if(visualNovels.begin(), visualNovels.end(),
		[](const VisualNovel& vn) {
			return vn.status == VisualNovel::Status::Finished;
		});
}

float OutputFileBuilder::meanRating()
{
	float totalRating = std::accumulate(visualNovels.begin(), visualNovels.end(), 0.0,
		[](float sum, const VisualNovel& vn) {
			return vn.status == VisualNovel::Status::Finished ? sum + vn.rating : sum;
		});

	return totalRating / this->getFinished();
}

int OutputFileBuilder::totalHoursPlayed()
{
	return std::accumulate(visualNovels.begin(), visualNovels.end(), 0,
		[](int sum, const VisualNovel& vn) {
			return vn.status == VisualNovel::Status::Finished ? sum + vn.playtime : sum;
		});
}

std::string OutputFileBuilder::BuildCurrentlyReadingVNs() {

	std::string output = this->StartTableRow();
	output.append(this->TableHeader("Currently reading"));
	output.append(this->EndTableRow());

	output.append(this->StartTableRow());

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Currently_Reading) {
			output.append(this->StartTableRow());
			output.append(this->TableData(visualNovel.name));
			output.append(this->EndTableRow());
		}
	}

	output.append(this->EndTableRow());

	return output;
}

std::string OutputFileBuilder::BuildOnHoldVNs() {

	std::string output = this->StartTableRow();
	output.append(this->TableHeader("On hold"));
	output.append(this->EndTableRow());

	output.append(this->StartTableRow());

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::On_Hold) {
			output.append(this->StartTableRow());
			output.append(this->TableData(visualNovel.name));
			output.append(this->EndTableRow());
		}
	}

	output.append(this->EndTableRow());

	return output;
}

std::string OutputFileBuilder::BuildPlanToReadVNs() {
	std::string output = this->StartTableRow();
	output.append(this->TableHeader("Plan to read"));
	output.append(this->EndTableRow());

	output.append(this->StartTableRow());

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Plan_To_Read) {
			output.append(this->StartTableRow());
			output.append(this->TableData(visualNovel.name));
			output.append(this->EndTableRow());
		}
	}

	output.append(this->EndTableRow());

	return output;
}

std::string OutputFileBuilder::BuildPlanToReadButCannotVNs() {

	std::string output = this->StartTableRow();
	output.append(this->TableHeader("Plan to read, but unreleased / unlocalized"));
	output.append(this->TableHeader("Comment"));
	output.append(this->EndTableRow());

	output.append(this->StartTableRow());

	for (VisualNovel& visualNovel : visualNovels)
	{
		if (visualNovel.status == VisualNovel::Status::Plan_To_Read_But_Cannot) {
			output.append(this->StartTableRow());
			output.append(this->TableData(visualNovel.name));
			output.append(this->TableData(visualNovel.comment));
			output.append(this->EndTableRow());
		}
	}

	output.append(this->EndTableRow());

	return output;
}


std::string OutputFileBuilder::Break()
{
	return "\n";
}

std::string OutputFileBuilder::Date()
{
	return "Last updated: Not added yet. Remember to add this.";
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
