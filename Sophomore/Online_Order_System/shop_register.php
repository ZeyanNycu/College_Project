<?php
    session_start();
    if(!isset($_SESSION['Authenticated']))
    {
    header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
    exit();
    }
    $dbservername = 'localhost';
	$dbusername = "root";
	$dbpassword = "Ab1824018)";
	$dbname="hw2";

    try{
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", $dbusername, $dbpassword);
        $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $name = $_POST['shopname'];
        $type = $_POST['category'];
        $longitude = $_POST['longitude'];
        $latitude = $_POST['latitude'];
        $uid = $_SESSION['UID'];

        if(empty($name))
            throw new Exception('shop name欄位空白');

        if(empty($type))
            throw new Exception('shop category欄位空白');
            
        if(empty($latitude))
            throw new Exception('latitude欄位空白');
           
        if(empty($longitude))
            throw new Exception('longitude欄位空白');

        if(!is_numeric($latitude) || !is_numeric($longitude)) 
            throw new Exception('經緯度為數字');

        if(floatval($latitude) < -90 || floatval($latitude) > 90) 
            throw new Exception('緯度格式錯誤');

        if(floatval($longitude) < -180 || floatval($longitude) > 180) 
            throw new Exception('經度格式錯誤');

        $sql = "select * from 店名 where 店名.店名 = :name";
        $stmt_2 = $conn->prepare($sql);
        $stmt_2->bindValue(':name',$name,PDO::PARAM_STR);
        $stmt_2->execute();
        if($stmt_2->rowCount() != 0)
            throw new Exception('店名已被註冊');
        //Select SID
        $sql = "select 手機 from 使用者 where 使用者.UID = '$uid'";
        $stmt_3 = $conn->prepare($sql);
        $stmt_3->execute();
        $phone = $stmt_3->fetch();
        $phonenumber = $phone["手機"];
        $la = $_POST['latitude'];
        $lo = $_POST['longitude'];
        $sql = "INSERT INTO 店名 (店名, 位置, 電話, 類別, UID,緯度,經度)
        VALUES ( :name ,point('$longitude','$latitude'), '$phonenumber', :type , $uid,$la,$lo)
        ";  
        $stmt_4 = $conn->prepare($sql);
        $stmt_4->bindValue(':name',$name,PDO::PARAM_STR);
        $stmt_4->bindValue(':type',$type,PDO::PARAM_STR);
        $stmt_4->execute(); 
        $_SESSION['Authenticated']=true;
        $sql = "select * from 店名 where UID = '$uid'";
        $stmt_3 = $conn->prepare($sql);
        $stmt_3->execute();
        $row = $stmt_3->fetch();
        $_SESSION['SID'] = $row['SID'];
        header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
        exit();
    }
        
    catch(Exception $e)
    {
        $msg=$e->getMessage();
        session_unset(); 
        session_destroy(); 
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