<?php

// TODO: Make vn name link to vndb page
// TODO: Rework URLs, write function that takes two paramters (text, link) that converts into MAL accepted format

// TODO: Classes for english, japanese & upcoming (prolly also helps with appending text to numbers, see todo :49)

function getMean(int ...$values): float
{
    return sprintf('%01.2f', array_sum($values) / count($values));
}

function url(string $text, string $url): string
{
    return "[url=$url]{$text}[/url]";
}

class VisualNovelBuilder
{
    private string $output = 'Since MyAnimeList does not support visual novels I will score them here.';

    /**
     * @param array<array{name:string,rating:int,hours:int,comment:string}> $english
     * @param array<array{name:string,hours:int,comment:string}> $japanese
     * @param array{"Currently reading": string[], "Next up (in order)": string[], "On hold / Restart": string[], "Dropped": string[], "Plan to read": array, "Plan to read, but unreleased": string[]} $upcoming
     */
    public function __construct(private array $english, private array $japanese, private array $upcoming) {}

    public function build(): void
    {
        $this->br();
        $this->date();
        $this->br();

        $this->buildEnglishTable();

        $this->br();

        $this->text("Visual Novels I've read in Japanese, unlocalized");
        $this->br();
        $this->text('I seperate these entries because I take a lot longer to read them, which would absolutely inflate the stats.');
        $this->buildJapaneseTable();
        $this->br();

        $this->buildUpcoming();

        // Write formatted file
        if (file_put_contents('VisualNovelList-FORMATTED2.md', $this->output) === false) {
            throw new Exception('Failed to write to destination file!');
        }
    }

    private function buildEnglishTable(): void
    {
        $this->output .= '[table]';

        $this->tr('th', 'Visual Novel', 'Rating', 'Play time', 'Comments');

        // TODO: Append ' hours' and '/10'
        foreach ($this->english as $vn) {
            $this->tr('td', ...$vn);
        }

        $stats = $this->englishStats();

        $this->tr(
            'th',
            $stats['games'] . ' GAMES',
            $stats['meanrating'] . '/10',
            $stats['hours'] . ' hours',
            'N/A'
        );

        $this->output .= '[/table]';
    }

    /**
     * @return array{games:int,meanrating:float,hours:int}
     */
    private function englishStats(): array
    {
        return [
            'games' => count($this->english),
            'meanrating' => getMean(...array_map(fn($vn) => $vn['rating'], $this->english)),
            'hours' => array_sum(array_map(fn($vn) => $vn['hours'], $this->english)),
        ];
    }


    private function buildJapaneseTable(): void
    {
        $this->output .= '[table]';

        $this->tr('th', 'Visual Novel', 'Play time', 'Comments');

        foreach ($this->japanese as $vn) {
            $this->tr('td', ...$vn);
        }

        $stats = $this->japaneseStats();

        $this->tr(
            'th',
            $stats['games'] . ' GAME',
            $stats['hours'] . ' hours',
            'N/A'
        );
        $this->output .= '[/table]';
    }

    /**
     * @return array{games:int,hours:int}
     */
    private function japaneseStats(): array
    {
        return [
            'games' => count($this->japanese),
            'hours' => array_sum(array_map(fn($vn) => $vn['hours'], $this->japanese)),
        ];
    }

    private function buildUpcoming(): void
    {
        $this->output .= '[table]';

        $this->tr('th', 'Currently reading');
        $this->tr('td', ...$this->upcoming['Currently reading']);

        $this->tr('th', 'Next up (in order)');
        $this->tr('td', ...$this->upcoming['Next up (in order)']);

        $this->tr('th', 'On hold / Restart');
        $this->tr('td', ...$this->upcoming['On hold / Restart']);

        $this->tr('th', 'Dropped');
        $this->tr('td', ...$this->upcoming['Dropped']);

        // TODO: Break array into chunks of 3
        $this->tr('th', 'Plan to read');
        $this->tr('th', '');
        $this->tr('th', '');
        foreach ($this->upcoming['Plan to read'] as $i => $vn) {
            $this->tr('td', $vn);
        }

        $this->tr('th', 'Plan to read, but unreleased');
        $this->tr('th', '');
        $this->tr('th', '');
        foreach ($this->upcoming['Plan to read, but unreleased'] as $i => $vn) {
            $this->tr('td', $vn);
        }

        $this->output .= '[/table]';
    }

    /**
     * Add linebreak
     */
    private function br(): void
    {
        $this->output .= "\n";
    }

    /**
     * @param string $kind th or td
     * @param string ...$entries
     */
    private function tr(string $kind, string ...$entries): void
    {
        $this->output .= "[tr]";

        foreach ($entries as $entry) {
            $this->output .= "[{$kind}]{$entry}[/{$kind}]";
        }

        $this->output .= '[/tr]';
    }

    private function date(): void
    {
        $this->output .= 'LAST UPDATED: ' . date('F jS Y');
    }

    private function text(string $text): void
    {
        $this->output .= $text;
    }
}

class VN {}

class EnglishVN extends VN {}

class JapaneseVN extends VN {}

$english = [
    [
        'name' => "Steins;Gate",
        'rating' => 8,
        'hours' => 34,
        'comment' => "PATCH USED: " . url('Committee of Zero Improvement Patch', 'https://sonome.dareno.me/projects/sghd.html')
    ],
    [
        'name' => "Steins;Gate 0",
        'rating' => 7,
        'hours' => 32,
        'comment' => "PATCH USED: " . url('Committee of Zero Improvement Patch', 'https://sonome.dareno.me/projects/sg0-steam.html'),
    ],
    [
        'name' => "Chaos;Head",
        'rating' => 6,
        'hours' => 28,
        'comment' => "PATCH USED: " . url('Chaotic;Head Patch', 'https://nipkownix.github.io/ChaoticHead')
    ],
    [
        'name' => "Robotics;Notes ELITE",
        'rating' => 8,
        'hours' => 42,
        'comment' => "PATCH USED: " . url('Committee of Zero Improvement Patch', 'https://sonome.dareno.me/projects/rne-steam.html'),
    ],
    [
        'name' => "Chaos;Child",
        'rating' => 9,
        'hours' => 54,
        'comment' => "PATCH USED: " . url('Committee of Zero Improvement Patch', 'https://sonome.dareno.me/projects/chaoschild-steam.html'),
    ],
    [
        'name' => "Fate/Stay Night (Realta Nua)",
        'rating' => 8,
        'hours' => 78,
        'comment' => "PATCH USED: " . url('Ultimate Edition Patch', 'https://forums.nrvnqsr.com/showthread.php/9101-Fate-Stay-Night-Realta-Nua-Ultimate-Edition-2022')
    ],
    [
        'name' => "Fate/Hollow Ataraxia",
        'rating' => 7,
        'hours' => 32,
        'comment' => "PATCH USED: " . url('Voice Patch', 'https://forums.nrvnqsr.com/showthread.php/7705-Fate-Hollow-Ataraxia-Voice-Patch')
    ],
    [
        'name' => "Wonderful Everyday Down the Rabbit-Hole",
        'rating' => 10,
        'hours' => 50,
        'comment' => "None"
    ],
    [
        'name' => "Phoenix Wright: Ace Attorney Trilogy",
        'rating' => 8,
        'hours' => 61,
        'comment' => "None"
    ],
    [
        'name' => "eden*",
        'rating' => 6,
        'hours' => 9,
        'comment' => "18+ DLC not available in Germany (LOL)",
    ],
    [
        'name' => "Chaos;Head Noah",
        'rating' => 8,
        'hours' => 47,
        'comment' => "PATCH USED: [url=https://sonome.dareno.me/projects/chn-patch.html]Committee of Zero Improvement Patch[/url]",
    ],
    [
        'name' => "AI: The Somnium Files",
        'rating' => 6,
        'hours' => 24,
        'comment' => "None",
    ],
    [
        'name' => "DeadΩAegis: Gaiden",
        'rating' => 3,
        'hours' => 2,
        'comment' => "Prequel to DeadΩAegis",
    ],
    [
        'name' => "Robotics;Notes DaSH",
        'rating' => 5,
        'hours' => 23,
        'comment' => "PATCH USED: " . url('Committee of Zero Improvement Patch', 'http://sonome.dareno.me/projects/rnd-steam.html')
    ],
    [
        'name' => "Anonymous;Code",
        'rating' => 7,
        'hours' => 16,
        'comment' => "None",
    ],
    [
        'name' => "Muv-Luv",
        'rating' => 7,
        'hours' => 46,
        'comment' => "PATCH USED: " . url("Muv-Luv Director's Cut Patch", 'https://drive.google.com/file/d/1IE1I9xAQlmFKcTay-cZhCiyRAoYgiXqy')
    ],
    [
        'name' => "Muv-Luv Alternative",
        'rating' => 10,
        'hours' => 41,
        'comment' => "PATCH USED: " . url("Muv-Luv Alternative Director's Cut Patch", 'https://drive.google.com/file/d/1VbSge69gXCi6Nn0R-bN7Uw0wNUEkXVk9')
    ],
    [
        'name' => "Tsukihime",
        'rating' => 6,
        'hours' => 34,
        'comment' => "Read on " . url('Tsukiweb', 'https://tsukiweb.holofield.fr'),
    ],
    [
        'name' => "Vampires' Melody",
        'rating' => 5,
        'hours' => 7,
        'comment' => "None"
    ],
    [
        'name' => "Lily of the Valley",
        'rating' => 6,
        'hours' => 2,
        'comment' => "None"
    ],
];

$japanese = [
    [
        'name' => '月影のシミュラクル',
        'hours' => 84,
        'comment' => url('High Level Complete Set Edition', 'https://vndb.org/r64111'),
    ],
    [
        'name' => 'eden*',
        'hours' => 33,
        'comment' => 'None',
    ],
];

$upcoming = [
    'Currently reading' => [],
    'Next up (in order)' => [],
    'On hold / Restart' => ['WITCH ON THE HOLY NIGHT (burnt out)'],
    'Dropped' => ['DeadΩAegis'],
    'Plan to read' => [
        'The Great Ace Attorney Chronicles',
        'Saya no Uta',
        'Senren＊Banka',
        'Apollo Justice: Ace Attorney Trilogy',
        'Danganronpa 1',
        'ATRI -My Dear Moments-',
        'Dies Irae',
        'Danganronpa 2',
        'Clannad',
        'Milk inside a bag of milk inside a bag of milk',
        'Danganronpa V3',
        'Tsukihime PLUS-DISC',
        'Milk outside a bag of milk outside a bag of milk',
        'Tsukihime Remake',
        'Higurashi: When They Cry',
        'Kagetsu Tohya',
        'Fox Hime Zero',
        'Umineko: When they cry',
        'BALDR SKY',
        'The Fruit of Grisaia',
        'Kara no Shojo',
        'White Album',
        'Kara no Shojo',
        'Wagamama High Spec',
        'Chaos;Head Love Chu Chu!',
        'Chaos;Child Love Chu Chu!!Kimi ga Nozomu Eien',
        'Sakura no Uta',
        'Occultic;Nine',
        'Kimi ga Ita Kisetsu',
        'White Album 2',
    ],
    'Plan to read, but unreleased' => [
        'Steins;??? (thematic sequel)',
        'Muv-Luv Integrate'
    ]
];

$builder = new VisualNovelBuilder($english, $japanese, $upcoming);

$builder->build();
