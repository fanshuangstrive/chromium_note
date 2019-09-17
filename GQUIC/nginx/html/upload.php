<?php
error_reporting(1);
print_r($_FILES); 

foreach($_FILES as $key => $value){
  
  var_dump($key);
  var_dump($value);
  $re=move_uploaded_file($_FILES[$key]["tmp_name"], "/home/fanshuang/nginx/html/upload/" . time() ."-".$_FILES[$key]["name"]);
  var_dump($re);
}

?>
