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
    try{
        if (!isset($_POST['value']))
        {
            header("Location: nav.php");
            exit();
        }
        if (empty($_POST['value']))
        throw new Exception('Please input money.');
        if(!is_numeric($_POST['value']) || $_POST['value']<0)
        {
            throw new Exception('Please input nature numbers');
        }
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
    $dbusername, $dbpassword);
        $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        $real = hash('sha256',$_SESSION['username']);
        $mon = $_POST['value']+$_SESSION['money'];
        $stmt=$conn->prepare("update 使用者 set  餘額=".$mon." where username='" .$real. "'");
        $stmt->execute();
        $_SESSION['money'] = $_POST['value']+$_SESSION['money'];
        $time = date("Y-m-d h:i:sa");
        $value = $_POST['value'];
        $UID = $_SESSION['UID'];
        $sql = "INSERT INTO 交易紀錄 (交易行為, 金額, 時間, UID, SID)
        VALUES ('儲值',$value,'$time',$UID,0)";
        $stmt=$conn->prepare($sql);
        $stmt->execute();
        throw new Exception('儲值成功');
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
        window.location.replace("https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
        </script>
        </body>
        </html>
    EOT;
    }
?>