<?php

try {
    $sourceFile = 'VisualNovelList.md';
    $destinationFile = 'VisualNovelList-FORMATTED.md';

    if (!file_exists($sourceFile)) {
        throw new Exception('Source file does not exist!');
    }

    // Read source file
    $contents = file_get_contents($sourceFile);

    // TODO: Somehow scrape rating and hours from file

    $english_stats = get_english_stats();
    $japanese_stats = get_japanese_stats();

    $mean = getMean(...$english_stats->ratings);
    $english_hours = array_sum($english_stats->playtime);
    $japanese_hours = array_sum($japanese_stats->playtime);

    // Remove all line breaks and tabs, and insert date and line breaks
    $search =   ["\n",  "\r",   '    ', '[br]', '#{DATE}',      '#{MEAN}',  '#{HOURS}',     '#{J_HOURS}'];
    $replace =  ['',    '',     '',     "\n",   date('F jS Y'), $mean,      $english_hours, $japanese_hours];
    $contents = str_replace($search, $replace, $contents);

    // Write formatted file
    if (file_put_contents($destinationFile, $contents) === false) {
        throw new Exception('Failed to write to destination file!');
    }

    echo "Everything worked fine!\n";
    echo "Mean: $mean\n";
    echo "English Hours: $english_hours\n";
    echo "Japanese Hours: $japanese_hours\n";
    echo "Total: " . $english_hours + $japanese_hours;
} catch (Exception $e) {
    echo 'Error: ' . $e->getMessage() . "\n";
}


function get_english_stats()
{
    return new VisualNovel(
        [8, 7, 6, 8, 9, 8, 7, 10, 8, 6, 8, 6, 3, 5, 7, 7, 10, 6, 5, 6],
        [34, 32, 28, 42, 54, 78, 32, 50, 61, 9, 47, 24, 2, 23, 16, 46, 41, 34, 7, 2]
    );
}

function get_japanese_stats()
{
    return new VisualNovel(null, [84]);
}


function getMean(int ...$values): float
{
    $x = array_sum($values) / count($values);
    return sprintf('%01.2f', $x);
}

class VisualNovel
{

    /**
     * @param ?int[] $ratings
     * @param int[] $playtime
     */
    public function __construct(public ?array $ratings, public array $playtime)
    {
        if (!is_null($ratings) && count($ratings) !== count($playtime)) {
            throw new Exception("Uneven amount of ratings and play time!");
        }
    }
}
