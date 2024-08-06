<?php
    $servername = 'localhost';
	$dbusername = "root";
	$dbpassword = "Ab1824018)";
	$db="hw2";

    try{
        $conn = new PDO("mysql:host=$servername;dbname=$db",$dbusername, $dbpassword);
    # set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        $name = $_POST['name'];
        $phonenumber = $_POST['phonenumber'];
        $account = hash('sha256',$_POST['Account']);
        $repassword = hash('sha256',$_POST['re-password']);
        $password = hash('sha256',$_POST['password']);
        $latitude = $_POST['latitude'];
        $longitude = $_POST['longitude'];
        $tmp = strstr($name, ' ');
        if(empty($name))
            throw new Exception('使用者名稱欄位空白');
        if(empty($phonenumber))
            throw new Exception('手機欄位空白');
        if(empty($_POST['Account']))
            throw new Exception('帳號欄位空白');
        if(empty($_POST['password']))
            throw new Exception('密碼欄位空白');
        if(empty($_POST['re-password']))
            throw new Exception('重複密碼欄位空白');
        if(empty($latitude))
            throw new Exception('緯度欄位空白');
        if(empty($longitude))
            throw new Exception('經度度欄位空白');
        
       
        if($password != $repassword)
            throw new Exception('密碼驗證 ≠ 密碼');

        if( $tmp == false )
        {
            if( !ctype_alnum($name) )
                throw new Exception('名字僅可包含大小寫英文與數字');
        }
        
        if($tmp != false){
            if( !ctype_alnum(explode(' ',$name)[0]) || !ctype_alnum(explode(' ',$name)[1]))
                throw new Exception('名字僅可包含大小寫英文與數字');

            if(explode(' ',$name)[2]!= null) 
                throw new Exception('名字僅可包含一個空格');
        }

        if( !ctype_alnum($account) )
            throw new Exception('帳號僅可包含大小寫英文與數字');

        if( !ctype_alnum($_POST['password']) )
            throw new Exception('密碼僅可包含大小寫英文與數字');

        if(strlen($phonenumber) != 10 || !ctype_digit($phonenumber)) 
            throw new Exception('電話號碼為10位數字');

        if(!is_numeric($latitude) || !is_numeric($longitude)) 
            throw new Exception('經緯度為數字');

        if(floatval($latitude) < -90 || floatval($latitude) > 90) 
            throw new Exception('緯度格式錯誤');

        if(floatval($longitude) < -180 || floatval($longitude) > 180) 
            throw new Exception('經度格式錯誤');

        $sql = "select * from 使用者 where 使用者.username = :account";
        $stmt=$conn->prepare($sql);
        $stmt->bindValue(':account',$account,PDO::PARAM_STR);
        $stmt->execute();
        if($stmt->rowCount()!= 0)
            throw new Exception('帳號已被註冊');

        $sql = "INSERT INTO 使用者 (username, 密碼, 名字, 身分, 經度, 緯度, 手機, 餘額)
        VALUES (:account, '$password', '$name', 0, $longitude, $latitude, '$phonenumber', 0)
        ";    
        $stmt=$conn->prepare($sql);
        $stmt->bindValue(':account',$account,PDO::PARAM_STR);
        $stmt->execute();
        throw new Exception('註冊成功');
    }
        
    catch(Exception $e)
    {
        $msg=$e->getMessage();
        if($msg == '註冊成功')
        { 
            echo <<<EOT
            <!DOCTYPE html>
            <html>
            <body>
            <script>
            alert("$msg");
            window.location.replace("index.html");
            </script>
            </body>
            </html>
        EOT;
        }
        else
        {
            echo <<<EOT
            <!DOCTYPE html>
            <html>
            <body>
            <script>
            alert("$msg");
            window.location.replace("sign-up.html");
            </script>
            </body>
            </html>
        EOT; 
        }
        
    }
?>