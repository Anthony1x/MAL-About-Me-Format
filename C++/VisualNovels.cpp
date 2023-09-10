#include "VisualNovel.h"
#include "OutputFileBuilder.h"
#include <fstream>

int main()
{
	std::vector<VisualNovel> visualNovels = {
		// Finished
		VisualNovel("Steins;Gate",                              8,	34,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Steins;Gate 0",                            7,	32,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Chaos;Head",                               6,	28,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Robotics;Notes ELITE",                     8,	42,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Chaos;Child",                              9,	54,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Fate/Stay Night (Realta Nua)",             8,	78,	"PATCH USED: Ultimate Edition Patch"),
		VisualNovel("Fate/Hollow Ataraxia",                     7,	32,	"PATCH USED: Voice Patch"),
		VisualNovel("Wonderful Everyday Down the Rabbit-Hole",  10, 50, "None"),
		VisualNovel("Phoenix Wright: Ace Attorney Trilogy",     8,	61,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("eden*",                                    6,	9,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("Chaos;Head Noah",                          8,	47,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("AI: The Somnium Files",                    6,	24,	"PATCH USED: Committee of Zero Improvement Patch"),
		VisualNovel("DeadΩAegis: Gaiden",						5,	2,	"Prequel to DeadΩAegis"),

		// Currently reading
		VisualNovel("Anonymous;Code",		VisualNovel::Status::Currently_Reading),

		// On hold
		VisualNovel("Robotics; Notes DaSH",	VisualNovel::Status::On_Hold),
		VisualNovel("Danganronpa 1",		VisualNovel::Status::On_Hold),
		VisualNovel("DeadΩAegis",			VisualNovel::Status::On_Hold),

		// Plan to read
		VisualNovel("The Great Ace Attorney Chronicles",	VisualNovel::Status::Plan_To_Read),
		VisualNovel("Saya no Uta",							VisualNovel::Status::Plan_To_Read),
		VisualNovel("Dies Irae",							VisualNovel::Status::Plan_To_Read),
		VisualNovel("White Album",							VisualNovel::Status::Plan_To_Read),
		VisualNovel("White Album 2",						VisualNovel::Status::Plan_To_Read),
		VisualNovel("Senren＊Banka",							VisualNovel::Status::Plan_To_Read),
		VisualNovel("Danganronpa 2",						VisualNovel::Status::Plan_To_Read),
		VisualNovel("Danganronpa V3",						VisualNovel::Status::Plan_To_Read),
		VisualNovel("Tsukihime",							VisualNovel::Status::Plan_To_Read),
		VisualNovel("Tsukihime Remake",						VisualNovel::Status::Plan_To_Read),

		// Plan to read, but unreleased / unlocalized
		VisualNovel("Chaos;Head Love Chu Chu!",		"Fan-localization announced",	VisualNovel::Status::Plan_To_Read_But_Cannot),
		VisualNovel("Chaos;Child Love Chu Chu!!",	"Fan-localization announced",	VisualNovel::Status::Plan_To_Read_But_Cannot),
		VisualNovel("Steins;??? (thematic sequel)",	"Unreleased",					VisualNovel::Status::Plan_To_Read_But_Cannot),
		VisualNovel("Sakura no Uta",				"Unlocalized",					VisualNovel::Status::Plan_To_Read_But_Cannot)
	};

	OutputFileBuilder builder(visualNovels);

	std::string output = builder.Build();

	std::ofstream outputfile;

	outputfile.open("list.md");
	outputfile << output;
	outputfile.close();

	std::cout << "Wrote file without issues.\n";

	return 0;
}
