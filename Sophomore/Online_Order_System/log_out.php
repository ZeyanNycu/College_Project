<?php
    session_start();
    session_unset(); 
    session_destroy(); 
    header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
    exit();
?>