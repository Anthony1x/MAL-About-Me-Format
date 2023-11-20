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

    /** @var int[] $x Rating out of 10 */
    $x = [8, 7, 6, 8, 9, 8, 7, 10, 8, 6, 8, 6, 4, 5, 7];
    /** @var int[] $y Hours played */
    $y = [34, 32, 28, 42, 54, 78, 32, 50, 61, 9, 47, 24, 2, 23, 16];

    // Remove all line breaks and tabs, and insert date and line breaks
    $search =   ["\n",  "\r",   '    ', '[br]', '#{DATE}',      '#{MEAN}',      '#{HOURS}'];
    $replace =  ['',    '',     '',     "\n",   date('F jS Y'), GetMean(...$x), array_sum($y)];
    $contents = str_replace($search, $replace, $contents);

    // Write formatted file
    if (file_put_contents($destinationFile, $contents) === false) {
        throw new Exception('Failed to write to destination file!');
    }

    echo "Everything worked fine!\n";
    echo "Mean: ". GetMean(...$x) . "\n";
    echo "Hours: ". array_sum($y);
} catch (Exception $e) {
    echo 'Error: ' . $e->getMessage() . "\n";
}

function GetMean(int ...$values): float
{
    $x = array_sum($values) / count($values);
    return sprintf('%01.2f', $x);
}
