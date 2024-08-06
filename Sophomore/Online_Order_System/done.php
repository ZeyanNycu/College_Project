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
                $sql = "update 訂單 set  訂單狀態='Finished',完成時間='".$time."' where OID=" .$OID;
                $stmt=$conn->prepare($sql);
                $stmt->execute();
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