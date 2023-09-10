#include "VisualNovel.h"
#include <algorithm>

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

VisualNovel::VisualNovel(std::string name, std::string comment, Status status) :
	name(name), rating(), playtime(), comment(comment), status(status)
{
}

std::string VisualNovel::GetRating()
{
	return std::to_string(this->rating).append("/10");
}

std::string VisualNovel::GetPlaytime()
{
	return std::to_string(playtime).append(" hours");
}