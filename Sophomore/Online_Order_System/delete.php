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
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", $dbusername, $dbpassword);
        $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $name = $_POST['mealname'];
        $sid = $_SESSION['SID'];
        
        $sql = "delete from 商品 
        where 商品.商品名稱 = '$name' and 商品.SID = $sid;"; 
        $stmt_2 = $conn->prepare($sql);
        $stmt_2->execute();
        $json = file_get_contents('search_result.json');
        $json_data = json_decode($json,true);
        $i = 0;
        $array = array();
        while($i<Count($json_data))
        {
            if($name != $json_data[$i]['product_name'] && $sid != $json_data[$i]['s'])
            {
                array_push($array, array('name' => $json_data[$i]['name'], 'category' => $json_data[$i][ 'category'], 'distance' => $json_data[$i]['distance'],'price'=>$json_data[$i]['price'],'product_name'=>$json_data[$i]['product_name'], 'number' =>$json_data[$i]['number'], 'picture' =>$json_data[$i]['picture'] ,'picture_type'=>$json_data[$i]['picture_type'],'m'=>$json_data[$i]['m'],'s'=>$json_data[$i]['s']));
            }
            $i = $i+1;
        }
        $json = json_encode($array);
        $bytes = file_put_contents("search_result.json", $json);
        header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
        exit();
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
        window.location.replace("nav.html");
        </script>
        </body>
        </html>
    EOT;
    }

?>