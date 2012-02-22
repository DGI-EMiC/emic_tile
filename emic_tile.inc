<?php

function emic_load_tile($pid) {
  global $user;
  $path = drupal_get_path('module', 'emic_tile');

  drupal_add_css($path . '/css/tile.css');
  drupal_add_js($path . '/js/tile.js');
  $emicUser = $user->name;
  $emicPass = $user->pass;
  $tile_url = "http://localhost/Tile/?file=http://localhost:8080/fedora/objects/$pid/datastreams/TILE_JSON/content&emicPass=$emicPass&emicUser=$emicUser";
  $html = '<iframe id ="tile-iframe"  src="' . $tile_url . '"  scrolling="no" frameborder="0" style="width: 100%; height: 800px;">Errors: Cannot load TILE</iframe>';

  return $html;
}

//todo - Determine whihc characters are causing lines to break


function emic_build_json($book_pid) {
  global $base_url;
  module_load_include('inc', 'islandora_book', 'book_pack_utils');
  module_load_include("inc", "fedora_repository", "api/fedora_item");
  $fedora_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora/');
  // build primary array
  $json['pages'] = array();
  $page_pids = get_page_pids($book_pid);
  // create fedora item to retrieve datastreams

  foreach ($page_pids as $pid) {
    $item = new Fedora_Item($pid);

    // get info for lines array
    $ocr = $item->get_datastream_dissemination('OCR');
    $ocr_lines = explode("\n", $ocr);
    $lines = array();
    $counter = 0;
    foreach ($ocr_lines as $ocr_line) {
      $ocr_line = str_replace("\\", "EMIC_SLASH", $ocr_line);
      $ocr_line = str_replace("'", "EMIC_SINGLE_QUOTE", $ocr_line);
      $lines[$counter]['text'] = (string) $ocr_line;
      $lines[$counter]['id'] = "line$counter";
      $counter++;
    }

    $page_array = array();
    $page_array['id'] = $pid;
    $page_array['url'] = $fedora_url . '/objects/' . $pid . '/datastreams/JPEG/content';
    $page_array['lines'] = $lines;
    $page_array['shapes'] = array();

    $json['pages'][] = $page_array;
  }
  $json['labels'] = array();


  $j_string = drupal_to_js($json);
  $j_string = preg_replace('/[^(\x20-\x7F)]*/', '', $j_string);
  $j_string = str_replace('\"', "EMIC_DOUBLE_QUOTE", $j_string);
  $j_string = str_replace("EMIC_SINGLE_QUOTE", "'", $j_string);
  $j_string = str_replace("EMIC_SLASH", "\\\\", $j_string);
  $j_string = str_replace("EMIC_DOUBLE_QUOTE", '\"', $j_string);
  $j_string = str_replace(array("\\x3c", "\\x3e", "\\x26"), array('', '', ''), $j_string);
  $j_string = addslashes($j_string);
  $flat_pid = str_replace(':', '_', $book_pid);
  $tile_file = file_directory_path() . "/tile/$flat_pid";
  if (!is_dir(file_directory_path() . "/tile")) {
    mkdir(file_directory_path() . "/tile");
  }
  file_put_contents($tile_file, $j_string);
  $book_item = new fedora_item($book_pid);
  $file_url = "$base_url/$tile_file";
  $book_item->add_or_modify_by_reference($file_url, 'TILE_JSON', "Tile Data", 'text/plain', FALSE, 'Modified by Islandora API');
  drupal_set_message("Added Tile data to $book_pid");
  file_delete($tile_file);
}

function create_emic_tile($form_id, $form_values) {
  if ($form_values['values']['create_tile_json']) {
    emic_build_json($form_values['values']['pid']);
  }
}

function emic_save_tile_data() {
  global $base_url;
  module_load_include("inc", "fedora_repository", "api/fedora_item");
  $json = $_POST['text'];
  $json = addslashes($json);
  $book_pid = preg_replace('/%3A/', ':', $_POST['file_pid']);
  $flat_pid = str_replace(':', '_', $book_pid);
  $tile_file = file_directory_path() . "/tile/$flat_pid";
  if (!is_dir(file_directory_path() . "/tile")) {
    mkdir(file_directory_path() . "/tile");
  }
  file_put_contents($tile_file, $json);
  $book_item = new fedora_item($book_pid);
  $file_url = "$base_url/$tile_file";
  $book_item->add_or_modify_by_reference($file_url, 'TILE_JSON', "Tile Data", 'text/plain', FALSE, 'Modified by Islandora API');
  file_delete($tile_file);

  return;
}