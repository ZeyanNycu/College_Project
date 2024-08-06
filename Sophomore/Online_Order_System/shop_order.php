<?php
    session_start();
    if(!isset($_SESSION['Authenticated']))
    {
    header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
    exit();
    }
    $servername = 'localhost';
	$dbusername = "root";
	$dbpassword = "Ab1824018)";
    $_SESSION['shop_option'] = $_REQUEST['Section'];
	$db="hw2";
    if(!$_SESSION['Authenticated']) 
    {
        echo '';
        exit();
    }
try
{   
    $conn = new PDO("mysql:host=$servername;dbname=$db", $dbusername, $dbpassword);
    $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
    $option = $_REQUEST['Section'];
    $uid = $_SESSION['UID'];
    $sql = "select * from 店名 where UID = $uid";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $row = $stmt->fetch();
    $_SESSION['my_sid'] = $row['SID'];
    $SID = $_SESSION['my_sid'];
    if($option == 'All')  $sql = "select * from 訂單 where SID = $SID";
    else $sql = "select * from 訂單 where SID = $SID and 訂單狀態 = '$option'";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $i = 0;
    $row = $stmt->fetchALL();
    $ans = "";
    $bool = false;
    while($i<$stmt->rowCount())
    {
        $ans = $ans."<tr>"."<th style=\"word-wrap:break-word;word-break:break-all;white-space: pre-wrap;\">".$row[$i]['OID']."</th>";
        $ans = $ans."<td >". $row[$i]['訂單狀態'].'</td>';
        $ans = $ans."<td >". $row[$i]['建立時間'].'</td>';
        if($option == 'Finished' || $option == 'Cancel') $ans = $ans."<td >". $row[$i]['完成時間'].'</td>';
        else  $ans = $ans."<td >". " ".'</td>';
        $ans = $ans."<td >". $row[$i]['店名'].'</td>';
        $ans = $ans."<td >". $row[$i]['商店金額'].'</td>';
        $ans = $ans."<td >". '<button type="button" class="btn btn-info " data-toggle="modal" data-target="#shop'.$row[$i]['OID'].'">Detail</button>'.'</td>';//detail
        if($row[$i]['訂單狀態'] == 'Not Finish')//cancel
        {
        $ans = $ans."<td >".'<input type="checkbox" name="'.$row[$i]['OID'].'" value="'.$row[$i]['OID'].'">'.'</td>';
        $bool = true;
        }
        else $ans = $ans."<td >". " ".'</td>';
        $i +=1;
        $ans = $ans."</tr>";
    }
    $i = 0;
    $ans = $ans."<tr>";
    if($bool)
    {
        $ans = $ans.'<td> <input type="submit" value="Delete" class="btn btn-primary" style="background-color: #BB0000;"></td>
        <td> <input type="submit" value="Done"   class="btn btn-primary" formaction="done.php" style="background-color: #00AA00;"></td>';
    }
    $ans = $ans."</tr>";
    echo $ans;
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