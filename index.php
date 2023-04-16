<?php
// Read source file
$contents = file_get_contents("VisualNovelList.md");

if (!$contents) throw new Exception("FILE NOT FOUND!");

// Remove all line breaks and tabs
$contents = str_replace("\n", "", $contents);
$contents = str_replace("\r", "", $contents);
$contents = str_replace("    ", "", $contents);

// Insert date when '#{DATE}' string has been found.
$contents = str_replace("#{DATE}", date('F dS Y'), $contents);

// Insert line breaks when [br] is found
$contents = str_replace("[br]", "\n", $contents);

// Write formatted file
file_put_contents("VisualNovelList-FORMATTED.md", $contents);

echo "Everything worked fine!";