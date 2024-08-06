<?php
    session_start();
    if(!isset($_SESSION['Authenticated']))
    {
    header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
    exit();
    }
    $dbservername='localhost';
    $dbname='hw2';
    $dbusername='root';
    $dbpassword='Ab1824018)';
try
{   
    $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
    $dbusername, $dbpassword);
    $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
    $latitude = $_POST['latitude'];
    $longitude = $_POST['longitude'];
    if(empty($latitude) || empty($longitude))   throw new Exception('欄位空白');
    if(!is_numeric($latitude) || !is_numeric($longitude)) 
        throw new Exception('經緯度為數字');
    if(floatval($latitude) < -90 || floatval($latitude) > 90) 
        throw new Exception('緯度格式錯誤');
    if(floatval($longitude) < -180 || floatval($longitude) > 180) 
        throw new Exception('經度格式錯誤');
    $real = hash('sha256',$_SESSION['username']);
    $sql = "update 使用者 set  經度=". $longitude.", 緯度=" . $latitude ." where username='" .$real. "'";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
    exit();
}
catch(Exception $e)
{
    $msg=$e->getMessage();
    echo <<<EOT
    <!DOCTYPE html>
    <html>
    <body>
    <script>
    alert("$msg");
    window.location.replace("nav.php");
    </script>
    </body>
    </html>
    EOT;
}
?>
