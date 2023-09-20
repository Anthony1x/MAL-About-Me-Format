#include "VisualNovel.h"
#include <algorithm>

VisualNovel::VisualNovel()
{
}

VisualNovel::VisualNovel(std::string name, uint8_t rating, uint8_t playtime, std::string comment) :
	name(name),
	rating(rating),
	playtime(playtime),
	comment(comment),
	status(Status::Finished)
{
}

VisualNovel::VisualNovel(std::string name, Status status) :
	name(name), rating(), playtime(), comment(), status(status)
{
}

VisualNovel::VisualNovel(std::string name, std::string comment) :
	name(name), rating(), playtime(), comment(comment), status(Plan_To_Read_But_Cannot)
{
}

const wxString VisualNovel::GetName() const {
	return wxString(this->name.c_str(), wxConvUTF8);
}

const std::string VisualNovel::GetRating() const
{
	return std::to_string(this->rating).append("/10");
}

const std::string VisualNovel::GetPlaytime() const
{
	return std::to_string(playtime).append(" hours");
}

const wxString VisualNovel::GetComment() const
{
	return wxString(this->comment.c_str(), wxConvUTF8);
}
