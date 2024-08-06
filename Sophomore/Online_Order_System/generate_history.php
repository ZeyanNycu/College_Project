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
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
    $dbusername, $dbpassword);
    # set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
    $time = date("Y-m-d h:i:sa");
    $UID = $_SESSION['UID'];
    $value = $_SESSION['sum'];
    $shop_value = $_SESSION['shop_sum'];
    $SID = $_SESSION['s'];
    if($_SESSION['money']<$value)
    {
        throw new Exception('餘額不足');
    }
    $sql = "INSERT INTO 交易紀錄 (交易行為, 金額, 時間, UID,SID)
            VALUES ('付款',$value,'$time',$UID,$SID)";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $sql = "select * from 使用者 where UID = $UID";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $row_my_self = $stmt->fetch();
    $mon = $row_my_self['餘額'] - $value;
    $real = hash('sha256',$_SESSION['username']);
    $stmt=$conn->prepare("update 使用者 set  餘額=".$mon." where username='" .$real. "'");
    $stmt->execute();
   
    $sql = "select * from 店名 where SID=$SID";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $row = $stmt->fetch();
    $a = $row['UID'];
    $sql = "select * from 使用者 where UID=$a";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $row_2 = $stmt->fetch();
    $sql = "INSERT INTO 交易紀錄 (交易行為, 金額, 時間, UID,SID)
    VALUES ('收款',$shop_value,'$time', $a,$UID)";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $mon = $row_2['餘額']+$shop_value;
    $real =$row_2['username'];
    $stmt=$conn->prepare("update 使用者 set  餘額=".$mon." where username='" .$real. "'");
    $stmt->execute();
    //訂單內容
    $order_1= $_SESSION['order_1'];
    $sum_product = $_SESSION['sum_product'];
    $deliver_sum = $_SESSION['deliver_sum'];
    $c_sum = $_SESSION['c_sum'];
    $shop_name = $row['店名'];
    $sql = "INSERT INTO 訂單 (UID,SID,訂單內容,訂單內容1,訂單內容2,訂單內容3, 建立時間, 店名,商店金額,金額,訂單狀態)
    VALUES ($UID,$SID,'$order_1','$sum_product','$deliver_sum','$c_sum','$time','$shop_name',$shop_value,$value,'Not Finish')";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    //處理數量問題
    $data = $order_1;
    $len = strlen($data);
    $split = 0;
    $bool = false;
    $a = "";
    $b = "";
    $bool_2 = false;
    $bool_3 = false;
    $j = 0;
    $json = file_get_contents('search_result.json');
    $json_data = json_decode($json,true);
    while($j<strlen($data))
    {
        if($data[$j] == '?' && $data[$j-1] =='>' && $data[$j+1] =='<' && $data[$j+2] =='/' && $data[$j+3] =='t' && $data[$j+4] =='d' && $data[$j+5] =='>')
        {
            $split = $j;
            $bool = true;
            $j = $j+11;
        }
        if($bool && $data[$j]=='<')
        {
            $bool = false;
            $bool_2 = true;
            $j = $j+8;
        }
        else if ($bool) $a = $a.$data[$j];
        if($bool_2 && $data[$j]=='<')
        {
            $bool_2 = false;
            $bool_3 = true;
            $j = $j+10;
        }
        if($bool_3 && $data[$j]=='<')
        {
            $bool_3 = false;
            $k = 0;
            while($k<Count($json_data))
            {
                if($a == $json_data[$k]['product_name'] && $SID == $json_data[$k]['s'])
                {
                    $int_temp = (int) $json_data[$k]['number'];
                    $int_b = (int) $b;
                    $int_ans = $int_temp-$int_b;
                    $json_data[$k]['number'] =  (string)$int_ans;
                    $mon = $json_data[$k]['number'];
                    $sql = "update 商品 set  庫存=".$mon." where SID=" .$SID. " and 商品名稱='".$a."'";
                    $stmt=$conn->prepare($sql);
                    $stmt->execute();
                }
                $k = $k+1;
            }
            $b = "";
            $a = "";
        }
        else if ($bool_3) $b = $b.$data[$j];
        $j=$j+1;
    }
    $json = json_encode($json_data);
    $bytes = file_put_contents("search_result.json", $json);
    throw new Exception('訂單成功');
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