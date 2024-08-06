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
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
    $dbusername, $dbpassword);
        $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        $sql = "select * from 訂單";
        $stmt_n=$conn->prepare($sql);
        $stmt_n->execute();
        $row_n = $stmt_n->fetchALL();
        $n = 0;
        while($n<$stmt_n->rowCount())
        {
            $index =  $row_n[$n]['OID'];
            if(isset($_POST[$index]))
            {
                $OID = $_POST[$index];
                $time = date("Y-m-d h:i:sa");
                $sql = "update 訂單 set  訂單狀態='Cancel',完成時間='".$time."' where OID=" .$OID;
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                $sql = "select * from 訂單 where OID=$OID";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                $row = $stmt->fetch();
                $SID = $row['SID'];
                $UID = $row['UID'];
                $my_UID = $_SESSION['UID'];
                $value = $row['金額'];
                $shop_value = $row['商店金額'];
                $data = $row['訂單內容'];
                //使用者
                //交易紀錄
                $sql = "INSERT INTO 交易紀錄 (交易行為, 金額, 時間, UID,SID)
                    VALUES ('給退款',$shop_value,'$time',$my_UID,$UID)";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                //餘額的更新
                $sql = "select * from 使用者 where UID=$my_UID";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                $row = $stmt->fetch();
                $mon = $row['餘額']-$shop_value;
                $real = hash('sha256',$_SESSION['username']);
                $stmt=$conn->prepare("update 使用者 set  餘額=".$mon." where username='" .$real. "'");
                $stmt->execute();
                //商家
                //交易紀錄
                $sql = "INSERT INTO 交易紀錄 (交易行為, 金額, 時間, UID,SID)
                VALUES ('退款',$value,'$time', $UID,$SID)";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                //使用者
                $sql = "select * from 使用者 where UID=$UID";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                $row_2 = $stmt->fetch();
                $mon = $row_2['餘額']+$value;
                $real = $row_2['username'];
                $sql = "update 使用者 set  餘額=".$mon." where username='" .$real. "'";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                //處理數量問題
                $len = strlen($data);
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
                                $int_ans = $int_temp+$int_b;
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
            }
            $n +=1;
        }
        header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
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