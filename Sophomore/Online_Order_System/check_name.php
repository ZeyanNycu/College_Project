<?php
session_start();
$dbservername='localhost';
$dbname='hw2';
$dbusername='root';
$dbpassword='Ab1824018)';
try {
    if (!isset($_REQUEST['Account']) || empty($_REQUEST['Account']))
    {
        echo 'FAILED';
        exit();
    }
    $uname=$_REQUEST['Account'];
    $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", $dbusername, $dbpassword);
    # set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    
    $want=hash('sha256',$_REQUEST['Account']);
    $stmt=$conn->prepare("select * from 使用者 where username=:username");
    $stmt->execute(array('username' => $want));
    if ($stmt->rowCount()==0)
    {
        echo 'YES';
    }
    else {
        echo 'NO';
    }
}
catch(Exception $e)
{ 
    echo 'FAILED';
}
?>
