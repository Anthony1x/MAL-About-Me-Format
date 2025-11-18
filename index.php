<?php

try {
    $data = json_decode(file_get_contents('visual-novels.json'), true);
    if (!$data) {
        throw new Exception("Could not read visual-novels.json");
    }

    $template = file_get_contents('VisualNovelList.md');
    if (!$template) {
        throw new Exception("Could not read VisualNovelList.md");
    }

    /* English rows */
    $english_rows = "";
    $ratings = [];
    $english_hours = 0;

    foreach ($data['english'] as $vn) {
        $title = $vn['title'];
        $rating = $vn['rating'];
        $hours = $vn['hours'];
        $comment = $vn['comment'];

        $ratings[] = $rating;
        $english_hours += $hours;

        $english_rows .= "[tr][td]{$title}[/td][td]{$rating}/10[/td][td]{$hours} hours[/td][td]{$comment}[/td][/tr]";
    }

    $mean = sprintf('%01.2f', array_sum($ratings) / max(count($ratings), 1));
    $english_count = count($ratings);


    /* Japanese rows */
    $japanese_rows = "";
    $japanese_hours = 0;

    foreach ($data['japanese'] as $vn) {
        $title = $vn['title'];
        $hours = $vn['hours'];
        $comment = $vn['comment'];

        $japanese_hours += $hours;

        $japanese_rows .= "[tr][td]{$title}[/td][td]{$hours} hours[/td][td]{$comment}[/td][/tr]";
    }

    $japanese_count = count($data['japanese']);


    /* Simple single-row lists */
    function simple_rows($arr) {
        $out = "";
        foreach ($arr as $e) {
            $out .= "[tr][td]{$e}[/td][/tr]";
        }
        return $out;
    }

    $currently_reading_rows = simple_rows($data['currently_reading']);
    $next_up_rows = simple_rows($data['next_up']);
    $on_hold_rows = simple_rows($data['on_hold']);
    $dropped_rows = simple_rows($data['dropped']);

    /* 3-column grouped lists */
    function grouped_rows_3($arr) {
        $rows = "";
        $chunks = array_chunk($arr, 3);
        foreach ($chunks as $group) {
            $tds = "";
            for ($i = 0; $i < 3; $i++) {
                $tds .= "[td]" . ($group[$i] ?? "") . "[/td]";
            }
            $rows .= "[tr]{$tds}[/tr]";
        }
        return $rows;
    }

    $plan_to_read_rows = grouped_rows_3($data['plan_to_read']);
    $unreleased_rows = grouped_rows_3($data['unreleased']);

    /* Replace placeholders */
    $search = [
        "\n",
        "\r",
        '    ',
        '[br]',
        '#{DATE}',
        '#{ENGLISH_ROWS}',
        '#{JAPANESE_ROWS}',
        '#{MEAN}',
        '#{ENGLISH_HOURS}',
        '#{JAPANESE_HOURS}',
        '#{ENGLISH_COUNT}',
        '#{JAPANESE_COUNT}',
        '#{CURRENTLY_READING_ROWS}',
        '#{NEXT_UP_ROWS}',
        '#{ON_HOLD_ROWS}',
        '#{DROPPED_ROWS}',
        '#{PLAN_TO_READ_ROWS}',
        '#{UNRELEASED_ROWS}',
    ];

    $replace = [
        '',
        '',
        '',
        "\n",
        date('F jS Y'),
        $english_rows,
        $japanese_rows,
        $mean,
        $english_hours,
        $japanese_hours,
        $english_count,
        $japanese_count,
        $currently_reading_rows,
        $next_up_rows,
        $on_hold_rows,
        $dropped_rows,
        $plan_to_read_rows,
        $unreleased_rows
    ];

    $output = str_replace($search, $replace, $template);

    file_put_contents('VisualNovelList-FORMATTED.md', $output);

    echo "Done.\n";
    echo "English mean: $mean\n";
    echo "English hours: $english_hours\n";
    echo "Japanese hours: $japanese_hours\n";
    echo "Combined hours: " . ($english_hours + $japanese_hours) . "\n";

} catch (Exception $e) {
    echo "Error: " . $e->getMessage() . "\n";
}
