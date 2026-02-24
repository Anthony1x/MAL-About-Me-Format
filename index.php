<?php

$userId = 'u253466';
$apiUrl = 'https://api.vndb.org/kana/ulist';

function fetchVndb($url, $data)
{
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
    curl_setopt($ch, CURLOPT_HTTPHEADER, ['Content-Type: application/json']);
    $response = curl_exec($ch);
    if (curl_errno($ch)) {
        throw new Exception('Curl error: ' . curl_error($ch));
    }

    return json_decode($response, true);
}

function getLengthVotes(string $userId)
{
    $cmd = 'curl -s -A "Mozilla/5.0" "https://vndb.org/' . $userId . '/lengthvotes"';
    $content = shell_exec($cmd);

    if (!$content || strpos($content, "Crawlers are not permitted") !== false) {
        return [];
    }

    $votes = [];
    preg_match_all('/<a href="\/v([0-9]+)"[^>]*>.*?<\/a><\/td><td class="tc3">([^<]+)(?:<span class="small">([^<]+)<\/span>)?<\/td>.*?<td class="tc5"><abbr class="icon-lang-([^"]+)"/s', $content, $matches, PREG_SET_ORDER);

    foreach ($matches as $match) {
        $vnid = 'v' . $match[1];
        $hoursPart = trim($match[2]);
        $minutesPart = isset($match[3]) ? trim($match[3]) : '';
        $lang = $match[4];

        $hours = 0;
        if (preg_match('/(\d+)h/', $hoursPart, $h)) {
            $hours += (int)$h[1];
        } elseif (preg_match('/(\d+)m/', $hoursPart, $m)) {
            $hours += (int)$m[1] / 60;
        }

        if (preg_match('/(\d+)m/', $minutesPart, $m)) {
            $hours += (int)$m[1] / 60;
        }

        if (!isset($votes[$vnid])) {
            $votes[$vnid] = [];
        }
        $votes[$vnid][] = [
            'hours' => round($hours, 2),
            'lang' => $lang
        ];
    }
    return $votes;
}

function getEnglishTitle(array $vn): string
{
    if (isset($vn['titles']) && is_array($vn['titles'])) {
        foreach ($vn['titles'] as $t) {
            if ($t['lang'] === 'en') {
                return $t['title'];
            }
        }
    }
    return $vn['title'];
}

function getJapaneseTitle(array $vn): string
{
    if (isset($vn['titles']) && is_array($vn['titles'])) {
        foreach ($vn['titles'] as $t) {
            if ($t['lang'] === 'ja') {
                return $t['title'];
            }
        }
    }
    return getEnglishTitle($vn);
}

$lengthVotes = getLengthVotes($userId);
$allResults = [];
$more = true;
$page = 1;

while ($more) {
    $data = [
        'user' => $userId,
        'fields' => 'vn.title, vn.alttitle, vn.titles.title, vn.titles.lang, vn.length_minutes, vn.released, vote, labels.id, started, finished',
        'results' => 100,
        'page' => $page
    ];
    $response = fetchVndb($apiUrl, $data);
    if (!$response || !isset($response['results'])) {
        break;
    }
    $allResults = array_merge($allResults, $response['results']);
    $more = $response['more'] ?? false;
    $page++;
}

$groups = [
    'english' => [],
    'japanese' => [],
    'currently_reading' => [],
    'on_hold' => [],
    'dropped' => [],
    'plan_to_read' => [],
    'unreleased' => []
];
$hidden_dropped_hours = 0;
$finished_vn_titles = [];

foreach ($allResults as $item) {
    $vn = $item['vn'];
    $labels = array_column($item['labels'], 'id');
    $vnId = $item['id'];
    $jaTitle = getJapaneseTitle($vn);
    $enTitle = getEnglishTitle($vn);
    $isImmersion = in_array(11, $labels);

    if (in_array(2, $labels)) { // Finished
        $finished_vn_titles[] = $jaTitle;
        $votesByLang = [];
        $allVotes = $lengthVotes[$vnId] ?? [['hours' => round($vn['length_minutes'] / 60, 2), 'lang' => 'unknown']];

        foreach ($allVotes as $v) {
            if (!isset($votesByLang[$v['lang']])) {
                $votesByLang[$v['lang']] = 0;
            }
            $votesByLang[$v['lang']] += $v['hours'];
        }

        foreach ($votesByLang as $lang => $hours) {
            $finishDate = $item['finished'];
            $targetList = ($lang === 'ja' || ($isImmersion && count($votesByLang) === 1)) ? 'japanese' : 'english';

            if ($vnId === 'v1286' && $lang === 'ja') {
                $finishDate = '2025-03-01';
            }

            $groups[$targetList][] = [
                'title' => ($targetList === 'japanese' ? $jaTitle : $enTitle),
                'rating' => $item['vote'] ? $item['vote'] / 10 : 0,
                'hours' => $hours,
                'finished' => $finishDate ?? '9999-99-99'
            ];
        }
    } elseif (in_array(4, $labels)) { // Dropped
        $groups['dropped'][] = $jaTitle;
        if (isset($lengthVotes[$vnId])) {
            foreach ($lengthVotes[$vnId] as $v) {
                $hidden_dropped_hours += $v['hours'];
            }
        }
    } elseif (in_array(1, $labels)) {
        $groups['currently_reading'][] = $jaTitle;
    } elseif (in_array(3, $labels)) {
        $groups['on_hold'][] = $jaTitle;
    } elseif (in_array(5, $labels)) {
        $released = $vn['released'];
        if (mb_strlen($released) !== 10 || ($released && $released > date('Y-m-d'))) {
            $groups['unreleased'][] = $jaTitle;
        } else {
            $groups['plan_to_read'][] = $jaTitle;
        }
    }
}

// Filter out dropped VNs that are also finished
$groups['dropped'] = array_filter($groups['dropped'], fn($title) => !in_array($title, $finished_vn_titles));

$dateSorter = fn($a, $b) => strcmp($a['finished'], $b['finished']);
usort($groups['english'], $dateSorter);
usort($groups['japanese'], $dateSorter);

function simple_rows(array $arr)
{
    if (empty($arr)) return "[tr][td]None[/td][/tr]";
    $out = "";
    foreach ($arr as $e) {
        $out .= "[tr][td]{$e}[/td][/tr]";
    }
    return $out;
}
function grouped_rows_3(array $arr)
{
    if (empty($arr)) return "[tr][td][/td][td][/td][td][/td][/tr]";
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

$english_rows = "";
$english_ratings = [];
$english_hours = 0;
foreach ($groups['english'] as $vn) {
    $english_ratings[] = $vn['rating'];
    $english_hours += $vn['hours'];
    $english_rows .= "[tr][td]{$vn['title']}[/td][td]{$vn['rating']}/10[/td][td]{$vn['hours']} hours[/td][/tr]";
}
$english_mean = sprintf('%01.2f', array_sum($english_ratings) / max(count($english_ratings), 1));

$japanese_rows = "";
$japanese_ratings = [];
$japanese_hours = 0;
foreach ($groups['japanese'] as $vn) {
    $japanese_ratings[] = $vn['rating'];
    $japanese_hours += $vn['hours'];
    $japanese_rows .= "[tr][td]{$vn['title']}[/td][td]{$vn['rating']}/10[/td][td]{$vn['hours']} hours[/td][/tr]";
}
$japanese_mean = sprintf('%01.2f', array_sum($japanese_ratings) / max(count($japanese_ratings), 1));

$total_ratings = array_merge($english_ratings, $japanese_ratings);
$total_mean = sprintf('%01.2f', array_sum($total_ratings) / max(count($total_ratings), 1));
$total_hours = $english_hours + $japanese_hours + $hidden_dropped_hours;

$template = file_get_contents('VisualNovelList.md');
$replace = [
    '#{DATE}' => date('F jS Y'),
    '#{ENGLISH_ROWS}' => $english_rows,
    '#{JAPANESE_ROWS}' => $japanese_rows,
    '#{ENGLISH_MEAN}' => $english_mean,
    '#{JAPANESE_MEAN}' => $japanese_mean,
    '#{TOTAL_MEAN}' => $total_mean,
    '#{ENGLISH_HOURS}' => round($english_hours, 2),
    '#{JAPANESE_HOURS}' => round($japanese_hours, 2),
    '#{TOTAL_HOURS}' => round($total_hours, 2),
    '#{ENGLISH_COUNT}' => count($groups['english']),
    '#{JAPANESE_COUNT}' => count($groups['japanese']),
    '#{TOTAL_COUNT}' => count($groups['english']) + count($groups['japanese']),
    '#{CURRENTLY_READING_ROWS}' => simple_rows($groups['currently_reading']),
    '#{ON_HOLD_ROWS}' => simple_rows($groups['on_hold']),
    '#{DROPPED_ROWS}' => grouped_rows_3($groups['dropped']),
    '#{PLAN_TO_READ_ROWS}' => grouped_rows_3($groups['plan_to_read']),
    '#{UNRELEASED_ROWS}' => grouped_rows_3($groups['unreleased']),
];

$output = str_replace(array_keys($replace), array_values($replace), $template);
$output = str_replace(["\n", "\r", "    "], "", $output);
$output = str_replace('[br]', "\n", $output);

file_put_contents('VisualNovelList-FORMATTED.md', $output);
echo "Done.\n";
