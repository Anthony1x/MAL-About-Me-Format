<?php

try {
    $sourceFile = "VisualNovelList.md";
    $destinationFile = "VisualNovelList-FORMATTED.md";

    if (!file_exists($sourceFile)) {
        throw new Exception("Source file does not exist!");
    }

    // Read source file
    $contents = file_get_contents($sourceFile);

    // Remove all line breaks and tabs, and insert date and line breaks
    $search =   ["\n",  "\r",   "    ", "#{DATE}",      "[br]"];
    $replace =  ["",    "",     "",     date('F dS Y'), "\n"];
    $contents = str_replace($search, $replace, $contents);

    // Write formatted file
    if (file_put_contents($destinationFile, $contents) === false) {
        throw new Exception("Failed to write to destination file!");
    }

    echo "Everything worked fine!";
} catch (Exception $e) {
    echo 'Error: ' . $e->getMessage();
}
