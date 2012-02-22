<?php

function load_emic_tile_data($pid) {
  guarantee_json($pid);
  $fedora_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');
  $file_url = $fedora_url . "/objects/$pid/datastreams/TILE_JSON/content";
  $file = file_get_contents($file_url);

  echo stripslashes($file);
}

function guarantee_json($pid) {
  module_load_include('inc', 'emic_tile', 'emic_tile');
  $fedora_url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');
  $file_url = $fedora_url . "/objects/$pid/datastreams/TILE_JSON/content";
  $file_headers = @get_headers($file_url);
  if ($file_headers[0] == 'HTTP/1.1 404 Not Found') {
    emic_build_json($pid);
  }
}

