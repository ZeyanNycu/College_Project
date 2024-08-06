<?php
    session_start();
    if(!isset($_SESSION['Authenticated']))
    {
    header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
    exit();
    }
    $i=0;
   $_SESSION['s'] = $_REQUEST['s'];
   $_SESSION['sum'] = $_REQUEST['sum'];
   $_SESSION['shop_sum'] =  $_REQUEST['shop_sum'];
   $_SESSION['order_1'] = $_REQUEST['order_1'];
   $_SESSION['sum_product'] = $_REQUEST['sum_product'];
   $_SESSION['deliver_sum'] = $_REQUEST['deliver_sum'];
   $_SESSION['c_sum'] = $_REQUEST['c_sum'];
   $_SESSION['num'] = $_REQUEST['num'];
   echo $_SESSION['c_sum'];
   exit();
?>