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
        $U = $_SESSION['UID'];
        $s=$_REQUEST['Section'];
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", $dbusername, $dbpassword);
        $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        if($s == 'All') $sql = "select * from 交易紀錄 where UID = $U";
        else if($s == '付款')$sql = "select * from 交易紀錄 where (交易行為 = '$s' or 交易行為 = '給退款') and UID = $U";
        else if($s == '收款')$sql = "select * from 交易紀錄 where (交易行為 = '$s' or 交易行為 = '退款') and UID = $U";
        else $sql = "select * from 交易紀錄 where 交易行為 = '$s' and UID = $U";
        $stmt=$conn->prepare($sql);
        $stmt->execute();
        $ans = "";
        if ($stmt->rowCount()!=0)
        {
            $row = $stmt->fetchALL();
            $i = 0;
            while($i<$stmt->rowCount())
            {
                $UID = $row[$i]['SID'];
                if($row[$i]['交易行為'] == '儲值')
                {
                    $sql = "select * from 使用者 where UID = $U";
                    $stmt_2=$conn->prepare($sql);
                    $stmt_2->execute();
                }
                else if ($row[$i]['交易行為'] == '付款' || $row[$i]['交易行為'] == '退款')
                {
                    $sql = "select * from 店名 where SID = $UID";
                    $stmt_2=$conn->prepare($sql);
                    $stmt_2->execute();
                }
                else
                {
                    $sql = "select * from 使用者 where UID = $UID";
                    $stmt_2=$conn->prepare($sql);
                    $stmt_2->execute();
                }
                $name = $stmt_2->fetch();
                $manner = $row[$i]['交易行為'];
                if($manner == '退款') $manner = '收款';
                else if($manner == '給退款') $manner = '付款';
                $ans = $ans."<tr>"."<th style=\"word-wrap:break-word;word-break:break-all;white-space: pre-wrap;\">".$row[$i]['TID']."</th>";
                $ans = $ans."<td >". $manner.'</td>';
                $ans = $ans."<td >". $row[$i]['時間'].'</td>';
                $ans = $ans."<td >". $name[1].'</td>';
                if($manner == '儲值' || $manner == '收款' )
                {
                    $ans = $ans."<td >+". $row[$i]['金額']."</td>";
                }
                else
                {
                    $ans = $ans."<td >-". $row[$i]['金額']."</td>";
                }
                $ans = $ans."</tr>";
                $i = $i+1;
            }
            echo $ans;
        }
        else
        {
            echo "";
        }
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