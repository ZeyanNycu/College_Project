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
    $sid = $_SESSION['SID'];
    if($option == 'All')  $sql = "select * from 訂單 where SID = $sid";
    else $sql = "select * from 訂單 where SID = $sid and 訂單狀態 = '$option'";
    $stmt=$conn->prepare($sql);
    $stmt->execute();
    $i = 0;
    $row = $stmt->fetchALL();
    $ans = "";
    $i = 0;
    while($i<$stmt->rowCount())
    {
        $data = $row[$i]['訂單內容'];
        $c = $row[$i]['訂單內容'];
        $s = $row[$i]['SID'];
        $j = 0;
        $len = strlen($data);
        $split = 0;
        $bool = false;
        $a = "";
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
                $sql = "select 圖片,圖片型態 from 商品 where SID =  $s and 商品名稱 = '$a'";
                $stmt_2=$conn->prepare($sql);
                $stmt_2->execute();
                $row_2 = $stmt_2->fetch();
                $type = $row_2['圖片型態'];
                $picture = $row_2['圖片'];
                $a = "";
                $copy = $data;
                $data = substr($copy,0,$split);
                $data = $data."<img src=\"data:'".$type."';base64, ". $picture ."\" />";
                $temp = substr($copy,$split+1,strlen($data)-1-$split);
                $data = $data.$temp;
                $j = 0;
            }
            else if ($bool) $a = $a.$data[$j];
            $j=$j+1;
        }
        $data1 = $row[$i]['訂單內容1'];
        $data2 = $row[$i]['訂單內容2'];
        $data3 = $row[$i]['訂單內容3'];
        $ans = $ans.'
    <div class="modal fade" id="shop'. $row[$i]['OID'].'"  data-backdrop="static" tabindex="-1" role="dialog" aria-labelledby="staticBackdropLabel" aria-hidden="true">
        <div class="modal-dialog">
            <div class="modal-content">
                <div class="modal-header">
                    <button type="button" class="close" data-dismiss="modal">&times;</button>
                    <h4 class="modal-title">Order</h4>
                </div>
                <div class="modal-body">
                    <!--  -->
                        <div class="row">
                            <div class="  col-xs-12">
                                <table class="table" style=" margin-top: 15px;">
                                    <thead>
                                        <tr>
                                        <th scope="col">Picture</th>
                                        <th scope="col">meal name</th>
                                        <th scope="col">price</th>
                                        <th scope="col">Quantity</th>
                                        </tr>
                                    </thead>
                                    <tbody>
                                        '.$data.'
                                    </tbody>
                                </table>
                            </div>
                        </div>
                    <!--  -->
                </div>
                <div class="modal-footer">
                    <div>
                        <t>'.$data1.' </t>
                        <Br></Br>
                        <t >'.$data2.' </t>
                        <Br></Br>
                        <t >'.$data3.' </t>
                    </div>
                 </div>
            </div>
        </div>
    </div>
                ';       
        $i =$i+1;
    }
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