<?php
session_start();
$_SESSION['Authenticated']=false;

$dbservername='localhost';
$dbname='hw2';
$dbusername='root';
$dbpassword='Ab1824018)';
try{
    if (!isset($_POST['Account']) || !isset($_POST['password']))
    {
        header("Location: index.html");
        exit();
    }
    if (empty($_POST['Account']) || empty($_POST['password']))
        throw new Exception('Please input user name and password.');
    $Account=hash('sha256',$_POST['Account']);
    $password=$_POST['password'];
    $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
    $dbusername, $dbpassword);
    # set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
    $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    $stmt=$conn->prepare("select * from 使用者 where username=:username");
    $stmt->execute(array('username' => $Account));
    $array = Array();
    $json = json_encode($array);
    $bytes = file_put_contents("search_result.json", $json);
    if ($stmt->rowCount()==1)
    {
        $row = $stmt->fetch();
        $sql = "select * from 店名 NATURAL JOIN 商品 where UID =".$row['UID'];
        $stmt_2 = $conn->prepare($sql);
        $stmt_2->execute();
        $row_2 = $stmt_2->fetchALL();
        $sql = "select * from 店名 where UID =".$row['UID'];
        $stmt_3 = $conn->prepare($sql);
        $stmt_3->execute();
        $row_3 = $stmt_3->fetchALL();
        if ($row['密碼']==hash('sha256',$_POST['password']))
        {
            if($stmt_3->rowCount()>0)
            {
                $SID = $row_3[0]['SID'];
            } 
            else $SID = '-1';
            $_SESSION['UID']=$row[0];
            $_SESSION['username']=$_POST['Account'];
            $_SESSION['name']=$row[3];
            $_SESSION['phonenumber']=$row[7];
            $_SESSION['latitude']=$row[6];
            $_SESSION['longitude']=$row[5];
            $_SESSION['money']=$row[8];
            $_SESSION['SID'] = $SID;
            if($SID != '-1')
            {
                $_SESSION['Authenticated']=true;
            }  
            header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
            exit();
        }
        else
            throw new Exception('登入失敗');
    }
    else
        throw new Exception("登入失敗");
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
    window.location.replace("https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
    </script>
    </body>
    </html>
EOT;
}
?>

