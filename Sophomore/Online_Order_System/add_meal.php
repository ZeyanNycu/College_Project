<?php
    $dbservername = 'localhost';
	$dbusername = "root";
	$dbpassword = "Ab1824018)";
	$dbname="hw2";
    session_start();
    if(!isset($_SESSION['Authenticated']))
    {
    header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
    exit();
    }
    try{
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
    $dbusername, $dbpassword);
    # set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
    $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $mealname = $_POST['mealname'];
        $quantity = $_POST['quantity'];
        $price = $_POST['price'];
        if(!file_exists($_FILES['photo']["tmp_name"]))
            throw new Exception('未上傳圖片');
        $imgType=$_FILES["photo"]["type"];
        $file = fopen($_FILES['photo']['tmp_name'], "rb") or die("error");
        $fileContents = fread($file, filesize($_FILES['photo']['tmp_name'])); 
        fclose($file);
        $fileContents = base64_encode($fileContents);
        if(empty($mealname) || empty($quantity) || empty($price))
            throw new Exception('欄位空白');

        if(!ctype_digit($quantity) || !ctype_digit($price))
            throw new Exception('價錢及數量為自然數');

        if(intval($quantity) < 0 || intval($price) < 0)
            throw new Exception('價錢及數量為自然數');

        if(explode($quantity,'0')[0] == null || explode($price,'0')[0] == null)
            throw new Exception('價錢及數量為自然數');

        //Select SID
        $sid = $_SESSION['SID'];
        $sql = "select * from 店名 where SID = $sid";
        $stmt_4 = $conn->prepare($sql);
        $stmt_4->execute();
        if($stmt_4->rowCount()>0)
        {
            $sql = "INSERT INTO 商品 (商品名稱, 價格, 庫存, 圖片, 圖片型態, SID)
            VALUES (:mealname , :price , :quantity , '$fileContents', '$imgType', '$sid')";
            $stmt_2 = $conn->prepare($sql);
            $stmt_2->bindValue(':mealname',$mealname,PDO::PARAM_STR);
            $stmt_2->bindValue(':price',$price,PDO::PARAM_INT);
            $stmt_2->bindValue(':quantity',$quantity,PDO::PARAM_INT);
            $stmt_2->execute();
            header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
            exit();
        }
        else
        {
            throw new Exception('請先註冊');
        }
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