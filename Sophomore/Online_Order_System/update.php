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
        # set the PDO error mode to exception
        $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $price = $_POST['price'];
        $quantity = $_POST['quantity'];
        $name = $_POST['mealname'];
        $sid = $_SESSION['SID'];

        if(empty($price))
            throw new Exception('price欄位空白');

        if(empty($quantity))
            throw new Exception('quantity欄位空白');

        if(!ctype_digit($quantity) || !ctype_digit($price))
            throw new Exception('價錢及數量為自然數');

        if(intval($quantity) < 0 || intval($price) < 0)
            throw new Exception('價錢及數量為自然數');

        if(explode($quantity,'0')[0] == null || explode($price,'0')[0] == null)
            throw new Exception('價錢及數量為自然數');
        
        $sql = "update 商品 set 價格 = $price, 庫存 = $quantity 
        where 商品.商品名稱 = '$name' and 商品.SID = '$sid';";   
        $stmt = $conn->prepare($sql);
        $stmt->execute();
        $json = file_get_contents('search_result.json');
        $json_data = json_decode($json,true);
        $k = 0;
        while($k<Count($json_data))
        {
            if($name == $json_data[$k]['product_name'] && $sid == $json_data[$k]['s'])
            {
                $json_data[$k]['number'] =  (string)$quantity;
                $json_data[$k]['price'] = (string)$price;
            }
            $k = $k+1;
        }
        $json = json_encode($json_data);
        $bytes = file_put_contents("search_result.json", $json);
        header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
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
        window.location.replace("nav.html");
        </script>
        </body>
        </html>
    EOT;
    }

?>