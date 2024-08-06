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
        $shop=$_POST['shop'];
        $distance=$_POST['sel1'];
        $a=$_POST['A'];
        $b=$_POST['B'];
        $Meal=$_POST['Meal'];
        $UID = $_SESSION['UID'];
        $category=$_POST['category'];
        $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", $dbusername, $dbpassword);
        $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
        $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
        $conn->exec("set names 'utf8'");
        $sql = "select * from 使用者 where UID = $UID";
        $stmt_n=$conn->prepare($sql);
        $stmt_n->execute();
        $row = $stmt_n->fetch();
        $my_loc_long = $row['經度'];
        $my_loc_lat = $row['緯度'];
        $sql = "select *,ST_Distance_Sphere(店名.位置 ,point('$my_loc_long','$my_loc_lat')) as m from 店名 NATURAL JOIN 商品 where ";
        
        if(!empty($shop))
        {
            $sql = $sql."店名.店名 like :shop and ";

        }
        if(!empty($distance))
        {
            if($distance == 'near')
            {
                $sql = $sql."ST_Distance_Sphere(店名.位置 ,point('$my_loc_long','$my_loc_lat'))<=10000 and ";
            }
            else if($distance == 'medium')
            {
                $sql = $sql."ST_Distance_Sphere(店名.位置 ,point('$my_loc_long','$my_loc_lat'))>10000 and ST_Distance_Sphere(店名.位置 ,point('$my_loc_long','$my_loc_lat'))<=100000 and ";
            }
            else if($distance == 'far')
            {
                $sql = $sql."ST_Distance_Sphere(店名.位置 ,point('$my_loc_long','$my_loc_lat'))>100000 and ";
            }
        }
        if(!empty($a))
        {
            $sql = $sql."商品.價格 between :a and 999999999 and ";
        }
        if(!empty($b))
        {
            $sql = $sql."商品.價格 between 0 and :b and ";
        }
        if(!empty($Meal))
        {
            $sql = $sql."商品.商品名稱 like :Meal and ";
        }
        if(!empty($category))
        {
            $sql = $sql."店名.類別 like :category and ";
        }
        $sql = $sql.'1';
        $stmt=$conn->prepare($sql);
        if(!empty($shop))
        {
            $shop = '%'.$shop.'%';
            $stmt->bindValue(':shop',$shop,PDO::PARAM_STR);
        } 
        if(!empty($a))
        {
            $stmt->bindValue(':a',$a,PDO::PARAM_INT);
        }
        if(!empty($b))
        {
            $stmt->bindValue(':b',$b,PDO::PARAM_INT);
        }
        if(!empty($Meal))
        {
            $Meal = '%'.$Meal.'%';
            $stmt->bindValue(':Meal',$Meal,PDO::PARAM_STR);
        }
        if(!empty($category))
        {
            $category = '%'.$category.'%';
            $stmt->bindValue(':category',$category,PDO::PARAM_STR);
        }
        $stmt->execute();
        if ($stmt->rowCount()!=0)
        {
            $row = $stmt->fetchALL();
            $i = 0;
            $array = array();
            while($i<$stmt->rowCount())
            {
                array_push($array, array('name' => $row[$i]['店名'], 'category' => $row[$i]['類別'], 'distance' => $distance,'price'=>$row[$i]['價格'],'product_name'=>$row[$i]['商品名稱'], 'number' =>$row[$i]['庫存'], 'picture' =>$row[$i]['圖片'] ,'picture_type'=>$row[$i]['圖片型態'],'m'=>$row[$i]['m'],'s'=>$row[$i]['SID']));
                $i = $i+1;
            }
            $json = json_encode($array);
            $bytes = file_put_contents("search_result.json", $json);
            header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/nav.php");
            exit();
        }
        else
        {
            $array = array();
            $json = json_encode($array);
            $bytes = file_put_contents("search_result.json", $json);
            throw new Exception('搜尋為空');
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