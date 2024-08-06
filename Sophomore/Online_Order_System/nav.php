<?php
session_start();
if(!isset($_SESSION['Authenticated']))
{
  header("Location: https://localhost/HW2_front_end_template/HW2_front_end_template/index.html");
  exit();
}
?>
<!doctype html>
<html lang="en">

<head>
  <!-- Required meta tags -->
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <!-- Bootstrap CSS -->

  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css">

  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js"></script>
  <title>Hello, world!</title>
  <script>
    function showresult_transaction(str) {
      if(str!="")
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          var message;
          if (this.readyState == 4 && this.status == 200) {
            message = this.responseText;
            document.getElementById("filt_transaction").innerHTML = message;
          }
        };
        xhttp.open("POST", "transaction.php", true);
        xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
        xhttp.send("Section="+str);
      }
      else	document.getElementById("filt_transaction").innerHTML = "";
    }
    function showresult_my_order(str) {
      if(str!="")
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          var message;
          if (this.readyState == 4 && this.status == 200) {
            message = this.responseText;
            document.getElementById("filt_my_order").innerHTML = message;
          }
        };
        xhttp.open("POST", "my_order.php", true);
        xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
        xhttp.send("Section="+str);
      }
      else	document.getElementById("filt_my_order").innerHTML = "";
      if(str!="")
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          var message;
          if (this.readyState == 4 && this.status == 200) {
            message = this.responseText;
            document.getElementById("detail_my_order").innerHTML = message;
          }
        };
        xhttp.open("POST", "detail_my_order.php", true);
        xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
        xhttp.send("Section="+str);
      }
      else	document.getElementById("detail_my_order").innerHTML = "";
    }
    function showresult_shop_order(str) {
      if(str!="")
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          var message;
          if (this.readyState == 4 && this.status == 200) {
            message = this.responseText;
            document.getElementById("filt_shop_order").innerHTML = message;
          }
        };
        xhttp.open("POST", "shop_order.php", true);
        xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
        xhttp.send("Section="+str);
      }
      else	document.getElementById("filt_shop_order").innerHTML = "";
      if(str!="")
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          var message;
          if (this.readyState == 4 && this.status == 200) {
            message = this.responseText;
            document.getElementById("detail_shop_order").innerHTML = message;
          }
        };
        xhttp.open("POST", "detail_shop_order.php", true);
        xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
        xhttp.send("Section="+str);
      }
      else	document.getElementById("detail_shop_order").innerHTML = "";
    }
    function check_name(uname)
    {
      if (uname!="")
      {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          var message;
          if (this.readyState == 4 && this.status == 200) {
            switch(this.responseText) {
              case 'YES':
              message='The shop name is available.';
              break;
              case 'NO':
              message='The shop name is not available.';
              break;
              default:
              message='Oops. There is something wrong.';
              break;
            }
            document.getElementById("msg").innerHTML = message;
          }
        };
        xhttp.open("POST", "check_shop_name.php", true);
        xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
        xhttp.send("Account="+uname);
      }
      else	document.getElementById("msg").innerHTML = "";
    }
</script>
</head>

<body>
 
  <nav class="navbar navbar-inverse">
    <div class="container-fluid">
      <div class="navbar-header">
        <a class="navbar-brand " href="#">WebSiteName</a>
      </div>

    </div>
  </nav>
  <div class="container">

    <ul class="nav nav-tabs">
      <li class="active"><a href="#home">Home</a></li>
      <li><a href="#menu1">shop</a></li>
      <li><a href="#menu2">MyOrder</a></li>
      <li><a href="#menu3">Shop Order</a></li>
      <li><a href="#menu4">Transation Record</a></li>
      <li><a href="log_out.php">Log out</a></li>
    </ul>

    <div class="tab-content">
      <div id="home" class="tab-pane fade in active">
        <h3>Profile</h3>
        <div class="row" display:inline>
          <div class="col-xs-12">
            <p id="demo" nowrap>
              <?php 
                $dbservername='localhost';
                $dbname='hw2';
                $dbusername='root';
                $dbpassword='Ab1824018)';
                $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
                $dbusername, $dbpassword);
                    $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
                    $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
                $UID = $_SESSION['UID'];
                $sql = "select * from 使用者 where UID = $UID";
                $stmt_n=$conn->prepare($sql);
                $stmt_n->execute();
                $row = $stmt_n->fetch();
                if($_SESSION["SID"] == -1)
                {
                  echo "Account: ";
                  echo $_SESSION['username'];
                  echo ", ".'user'.", "."Phonenumber: ".$row['手機'].", location: ".$row['緯度'].", ".$row['經度'];
                }
                else
                {
                  echo "Account: ".$_SESSION['username'].", ".'manager'.", "."Phonenumber: ".$row['手機'].", location: ".$row['緯度'].", ".$row['經度'];
                }
              ?></nobr></p>
            
            <button  nowrap type="button " style="margin-left: 5px;" class=" btn btn-info " data-toggle="modal"
            data-target="#location">edit location</button>
            <!--  -->
            <form action="update_loc.php" method="post"  class="fh5co-form animate-box" data-animate-effect="fadeIn">
              <div class="modal fade" id="location"  data-backdrop="static" tabindex="-1" role="dialog" aria-labelledby="staticBackdropLabel" aria-hidden="true">
                <div class="modal-dialog  modal-sm">
                  <div class="modal-content">
                    <div class="modal-header">
                      <button type="button" class="close" data-dismiss="modal">&times;</button>
                      <h4 class="modal-title">edit location</h4>
                    </div>
                    <div class="modal-body">
                      <label class="control-label " for="latitude">latitude</label>
                      <input type="text" class="form-control" id="latitude" placeholder="enter latitude" name = 'latitude'>
                        <br>
                        <label class="control-label " for="longitude">longitude</label>
                      <input type="text" class="form-control" id="longitude" placeholder="enter longitude" name = 'longitude'>
                    </div>
                    <div class="modal-footer">
                      <button type="submit" class="btn btn-default" data-dismiss="Edit">Edit</button>
                    </div>
                  </div>
                </div>
              </div>
            </form>



            <!--  -->
            <p id ="demo2">
            <?php 
                $dbservername='localhost';
                $dbname='hw2';
                $dbusername='root';
                $dbpassword='Ab1824018)';
                $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
                                $dbusername, $dbpassword);
                # set the PDO error mode to exception
                $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
                $UID = $_SESSION['UID'];
                $sql = "select * from 使用者 where UID=$UID";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                $row = $stmt->fetch();
                $money = $row['餘額'];
                echo 'walletbalance: '.$money;
              ?></p>
            <!-- Modal -->
            <button type="button " style="margin-left: 5px;" class=" btn btn-info " data-toggle="modal"
              data-target="#myModal">Recharge</button>
            <form action="update_value.php" method="post"  class="fh5co-form animate-box" data-animate-effect="fadeIn">
              <div class="modal fade" id="myModal"  data-backdrop="static" tabindex="-1" role="dialog" aria-labelledby="staticBackdropLabel" aria-hidden="true">
                <div class="modal-dialog  modal-sm">
                  <div class="modal-content">
                    <div class="modal-header">
                      <button type="button" class="close" data-dismiss="modal">&times;</button>
                      <h4 class="modal-title">Add value</h4>
                    </div>
                    <div class="modal-body">
                      <input type="text" class="form-control" id="value" placeholder="enter add value" name="value">
                    </div>
                    <div class="modal-footer">
                      <button type="submit" class="btn btn-default" data-dismiss="Add" >Add</button>
                    </div>
                  </div>
                </div>
              </div>
            </form>
          </div>
        </div>
        <!-- 
                
             -->
        <h3>Search</h3>
        <div class=" row  col-xs-8">
          <form class="form-horizontal" action="action_page.php" method="post">
            <div class="form-group">
              <label class="control-label col-sm-1" for="Shop">Shop</label>
              <div class="col-sm-5">
                <input type="text" class="form-control" placeholder="Enter Shop name" name="shop">
              </div>
              <label class="control-label col-sm-1" for="distance">distance</label>
              <div class="col-sm-5">


                <select class="form-control" id="sel1" name="sel1">
                  <option>near</option>
                  <option>medium </option>
                  <option>far</option>

                </select>
              </div>

            </div>

            <div class="form-group">

              <label class="control-label col-sm-1" for="Price">Price</label>
              <div class="col-sm-2">

                <input type="text" class="form-control" name="A">

              </div>
              <label class="control-label col-sm-1" for="~">~</label>
              <div class="col-sm-2">

                <input type="text" class="form-control" name="B">

              </div>
              <label class="control-label col-sm-1" for="Meal">Meal</label>
              <div class="col-sm-5">
                <input type="text" list="Meals" class="form-control" id="Meal" placeholder="Enter Meal" name="Meal">
                <datalist id="Meals">
                  <option value="Hamburger">
                  <option value="coffee">
                  <option value="rice">
                  <option value="noodle">
                </datalist>
              </div>
            </div>

            <div class="form-group">
              <label class="control-label col-sm-1" for="category"> category</label>
            
              
                <div class="col-sm-5">
                  <input type="text" list="categorys" class="form-control" id="category" placeholder="Enter shop category" name="category">
                  <datalist id="categorys">
                    <option value="fast food">
                    <option value="Chinese">
                    <option value="Janpanese">
                    <option value="Korea">
                    </option>
                  </datalist>
                </div>
                <button type="submit" style="margin-left: 18px;"class="btn btn-primary">Search</button>
              
            </div>
          </form>
        </div>
        <div class="row">
          <div class="  col-xs-8" >
            <div class="tab-content" id="result"></div>
                      <script type = 'text/javascrupt' src="jquery-1.9.1.min.js"></script>
                      <script nowrap>
                        a= $.ajax({
                          url:"search_result.json",
                          type:"GET",
                          datatype:'json',
                          async: false,
                          success: function(data){
                          }
                        })
                        aa=a.responseText;
                        result = $.parseJSON(a["responseText"]);
                        i=0;
                        cnt=0;
                        page = 1;
                        temp = "";
                        let pre="";
                        j = 0;
                        cnt_j = 1;
                        const arr_2 = [];
                        const arr = [];
                        j = 0;
                        type = 1;
                        k=1;
                        arr_2.push(result[0]['name']);
                        while(k<result.length)
                        {
                            b = true;
                            l = 0;
                            while(l<arr_2.length)
                            {
                              if(arr_2[l] == result[k]['name'])
                              {
                                b = false;
                                break;
                              }
                              l+=1;
                            }
                            if(b)  type +=1;
                            k+=1;
                        }
                        temp+='<table class="table" style=" margin-top: 15px;">\
                                <thead>\
                                   <tr>\
                                      <th scope="col">#</th>\
                                      <th scope="col">shop name</th>\
                                      <th scope="col">shop category</th>\
                                      <th scope="col">Distance</th>\
                                  </tr>\
                                </thead>\
                                <tbody>';
                        while(i<result.length)
                        { 
                          j = 0;
                          sign = true;
                          while(j<arr.length)
                          {
                              if(arr[j] == result[i]['name']) 
                              {
                                sign = false;
                                break;
                              }
                              j+=1;
                          }
                          if(sign)
                          {
                            a = cnt+1;
                            temp = temp+"<tr>";
                            temp = temp+"<th style=\"word-wrap:break-word;word-break:break-all;white-space: pre-wrap;\">"+a+"</th>";
                            temp = temp+"<td >"+result[i]['name']+'</td>';
                            temp = temp+"<td >"+result[i]['category']+'</td>';
                            temp = temp+"<td >"+result[i]['distance']+'</td>';
                            temp+='<td >  <button type="button" class="btn btn-info " data-toggle="modal" data-target="#'+result[i]['name']+'">Open menu</button></td>';
                            temp = temp+"</tr>";
                            cnt=cnt+1;
                            arr.push(result[i]['name']);
                          }
                          i+=1;
                        }
                        temp+=' \
                              </tbody>\
                            </table></div>'; 
                        
                        document.getElementById("result").innerHTML = temp;
                      </script>
            </div>
                <!-- Modal -->
  <div id="result_men">
  
                <script type = 'text/javascrupt' src="jquery-1.9.1.min.js"></script>
                      <script nowrap>
                        a= $.ajax({
                          url:"search_result.json",
                          type:"GET",
                          datatype:'json',
                          async: false,
                          success: function(data){
                          }
                        })
                        $(function() {
                            // This button will increment the value
                            $('.qtyplus').click(function(e) {
                              // Stop acting like a button
                              e.preventDefault();
                              // Get the field name
                              idea = $.ajax({
                                          url:"search_result.json",
                                          type:"GET",
                                          datatype:'json',
                                          async: false,
                                          success: function(data){
                                          }
                                        })
                              data = $.parseJSON(idea["responseText"]);
                              i=0;
                              msg = "";
                              sum = 0;
                              fieldName = $(this).attr('field');
                              Name = $(this).attr('name');
                              while(i<data.length)
                              {
                                if(data[i]['s'] == Name && data[i]['product_name'] == fieldName)
                                {
                                  var currentVal = parseInt($('input[name=' + fieldName + ']').val());
                                  // If is not undefined
                                  if(currentVal<data[i]['number'])
                                  {
                                    if (!isNaN(currentVal)) {
                                    // Increment
                                      $('input[name=' + fieldName + ']').val(currentVal + 1);
                                    } else {
                                      // Otherwise put a 0 there
                                      $('input[name=' + fieldName + ']').val(0);
                                    }
                                  }
                                  break;
                                }
                                i = i+1;
                              }
                              // Get its current value
                              
                            });
                            // This button will decrement the value till 0
                            $(".qtyminus").click(function(e) {
                              // Stop acting like a button
                              e.preventDefault();
                              // Get the field name
                              fieldName = $(this).attr('field');
                              // Get its current value
                              var currentVal = parseInt($('input[name=' + fieldName + ']').val());
                              // If it isn't undefined or its greater than 0
                              if (!isNaN(currentVal) && currentVal > 0) {
                                // Decrement one
                                $('input[name=' + fieldName + ']').val(currentVal - 1);
                              } else {
                                // Otherwise put a 0 there
                                $('input[name=' + fieldName + ']').val(0);
                              }
                            });
                            $(".test").click(function(e) {
                              // Stop acting like a button
                              e.preventDefault();
                              // Get the field name
                              fieldName = $(this).attr('field');
                              
                              // Get its current value
                              idea = $.ajax({
                                          url:"search_result.json",
                                          type:"GET",
                                          datatype:'json',
                                          async: false,
                                          success: function(data){
                                          }
                                        })
                              data = $.parseJSON(idea["responseText"]);
                              i=0;
                              msg = "";
                              ans = "";
                              sum = 0;
                              var state = $('select[name="' + fieldName + '"]').val();
                              num = 0;
                              while(i<data.length)
                              {
                                if(data[i]['name'] == fieldName)
                                {
                                  var currentVal = parseInt($('input[name=' + data[i]['product_name'] + ']').val());
                                  var price = parseInt(data[i]['price']);
                                  var fee = parseFloat(data[i]['m']);
                                  var s = parseInt(data[i]['s']);
                                  num+=1;
                                  if (!isNaN(currentVal) && currentVal > 0) {
                                    // Decrement one
                                    msg += "<tr>";
                                    msg += "<td > <img src=\"data:'"+data[i]['picture_type']+"';base64, "+ data[i]['picture'] +"\" /> </td>";
                                    msg += "<td >"+data[i]['product_name']+'</td>';
                                    msg += "<td >"+price+'</td>';
                                    sum = sum+currentVal*price;
                                    msg += "<td >"+currentVal+'</td>';
                                    msg += "</tr>";
                                    ans +=  "<tr>";
                                    ans += "<td >?</td>";
                                    ans += "<td >"+data[i]['product_name']+'</td>';
                                    ans += "<td >"+price+'</td>';
                                    ans += "<td >"+currentVal+'</td>';
                                    ans += "</tr>";
                                  }    
                                }
                                i=i+1;
                              }
                              document.getElementById("order_1").innerHTML = msg;
                              document.getElementById("sum_product").innerHTML = "Subtotal    "+sum;
                              msg3 = "Subtotal    "+sum;
                              shop_value = sum;
                              if(state == 'Delivery')
                              {
                                fee = (fee/100);
                                fee = fee+0.5;
                                fee = parseInt(fee);
                                sum = sum+fee;
                                msg1 = "Deliver fee    "+fee;
                                msg2 = "Total    "+sum;
                                document.getElementById("deliver_sum").innerHTML = msg1;
                                document.getElementById("sum").innerHTML = "Total    "+sum;
                              }
                              else
                              {
                                msg1 = "Deliver fee    0";
                                msg2 = "Total    "+sum;
                                document.getElementById("deliver_sum").innerHTML = "Deliver fee    0";
                                document.getElementById("sum").innerHTML = "Total    "+sum;
                              }
                              var xhttp = new XMLHttpRequest();
                              xhttp.onreadystatechange = function() {
                                var message;
                                  if (this.readyState == 4 && this.status == 200) {
                                    message = this.responseText;
                                    document.getElementById("sum").innerHTML = message;
                                  }
                              };
                              user_value = sum;
                              xhttp.open("POST", "make_json.php", true);
                              xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
                              xhttp.send("sum="+user_value+"&shop_sum="+shop_value+"&s="+s+"&order_1="+ans+"&sum_product="+msg3+"&deliver_sum="+msg1+"&c_sum="+msg2+"&num="+num);                  
                            });
                          });
                        aa=a.responseText;
                        result = $.parseJSON(a["responseText"]);
                        i=0;
                        cnt=1;
                        temp = "";
                        temp+='';
                        while(i<result.length)
                        {
                          if(result[i]['name'] != '-1')
                          {
                            cnt = 1;
                            temp =temp+'<div class="modal fade" id="'+ result[i]['name']+'"  data-backdrop="static" tabindex="-1" role="dialog" aria-labelledby="staticBackdropLabel" aria-hidden="true">\
    <div class="modal-dialog">\
      <!-- Modal content-->\
      <div class="modal-content">\
        <div class="modal-header">\
          <button type="button" class="close" data-dismiss="modal">&times;</button>\
          <h4 class="modal-title">menu</h4>\
        </div>\
        <div class="modal-body">\
         <!--  -->\
         <div class="row">\
          <div class="  col-xs-12">\
            \
            <table class="table" style=" margin-top: 15px;">\
              <thead>\
                <tr>\
                  <th scope="col">#</th>\
                  <th scope="col">Picture</th>\
                  <th scope="col">meal name</th>\
                  <th scope="col">price</th>\
                  <th scope="col">Quantity</th>\
                  <th scope="col">Order check</th>\
                </tr>\
              </thead>\
              <tbody id="result_menu">';
                          j = i;
                          while(j<result.length)
                          {
                            if(result[i]['name'] == result[j]['name'])
                            {
                              temp = temp+"<tr>";
                              temp = temp+"<th>"+cnt+"</th>";
                              temp = temp+"<td > <img src=\"data:'"+result[j]['picture_type']+"';base64, "+ result[j]['picture'] +"\" /> </td>";
                              temp = temp+"<td >"+result[j]['product_name']+'</td>';
                              temp = temp+"<td >"+result[j]['price']+'</td>';
                              temp = temp+"<td >"+result[j]['number']+'</td>';
                              temp = temp+"<td> <input type='button' value='-' class='qtyminus' field='"+result[j]['product_name']+"'></td><td ><input  size='2' type='text' name='"+result[j]['product_name']+"' value='0' class='qty' readonly /></td><td><input type='button' value='+' class='qtyplus' field='"+result[j]['product_name']+"' name="+result[j]['s']+"></td>";
                              temp = temp+"</tr>";
                              cnt=cnt+1;
                              if(i!=j) result[j]['name'] = '-1';
                            }
                            j+=1;
                          }
                          temp +='</tbody>\
                        </table>\
                        \
                        <label >Type</label>\
                        <select name='+result[i]['name']+'>\
                            <option value="Delivery">Delivery</option>\
                            <option value="Pick-up" selected >Pick-up</option>\
                        </select>\
                      </div>\
                    </div>\
                      <!--  -->\
                    </div>\
                    <div class="modal-footer">\
                    <input type="submit" data-toggle="modal" data-target="#order" value = "Calculate the price" class="test" field="'+result[i]['name']+'"></input>\
                    </div>\
                  </div>\
                  </form>\
                </div>\
                </div>\
                ';
                }
                i+=1;
                        }
                        
                        document.getElementById("result_men").innerHTML = temp;
                  </script>
              
  </div>
    <div class="modal fade" id="order"  data-backdrop="static" tabindex="-1" role="dialog" aria-labelledby="staticBackdropLabel" aria-hidden="true">
      <div class="modal-dialog">
        <!-- Modal content-->
        <div class="modal-content">
          <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal">&times;</button>
            <h4 class="modal-title">Order</h4>
          </div>
          <div class="modal-body">
            <table class="table" style=" margin-top: 15px;">
              <thead>
                <tr>
                  <th scope="col">Picture</th>
                  <th scope="col">meal name</th>
                  <th scope="col">price</th>
                  <th scope="col">Order Quality</th>
                </tr>
              </thead>
              <tbody id='order_1'>
                    <td>hello</td>
              </tbody>
            </table>
          </div>
          <div class="modal-footer">
            <div >
              <t id="sum_product"> </t>
              <Br></Br>
              <t id="deliver_sum">123 </t>
              <Br></Br>
              <t id="sum"> </t>
            </div>
            <form class="form-horizontal" action="generate_history.php" method="post">
              <input style=" margin-top: 15px;" type="submit" value="Order" class="btn btn-primary"></button>
            </form>
          </div>
        </div>
      </div>
    </div>
  </div>
                      </div>
    <div id="menu1" class="tab-pane fade" >
          <?php
              $dbservername = 'localhost';
              $dbusername = "root";
              $dbpassword = "Ab1824018)";
              $dbname="hw2";
              $SID = $_SESSION['SID'];
              if(!$_SESSION['Authenticated'])
              {
                echo '<h3> Start a business </h3>
                      <form action="shop_register.php" method="post" class="fh5co-form animate-box" data-animate-effect="fadeIn">
                        <div class="form-group ">
                          <div class="row">
                            <div class="col-xs-2">
                              <label for="ex5">shop name</label>
                              <input class="form-control" name="shopname" id="ex5" placeholder="macdonald" type="text" oninput="check_name(this.value)">
                              <label id="msg"></label>
                            </div>
                            <div class="col-xs-2">
                              <label for="ex5">shop category</label>
                              <input class="form-control" name="category" id="ex5" placeholder="fast food" type="text" >
                            </div>
                            <div class="col-xs-2">
                              <label for="ex6">latitude</label>
                              <input class="form-control"  name="latitude" id="ex6" placeholder="24.78472733371133" type="text" >
                            </div>
                            <div class="col-xs-2">
                              <label for="ex8">longitude</label>
                              <input class="form-control"  name="longitude" id="ex8" placeholder="121.00028167648875" type="text" >
                            </div>
                          </div>
                          <div class=" row" style=" margin-top: 25px;">
                            <div class=" col-xs-3">
                              <input type="submit" class="btn btn-primary" name="register" value="register" ></input>
                            </div>
                          </div>
                        </div>
                      </form>';
              }
              else
              {
                $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
                                  $dbusername, $dbpassword);
                                  # set the PDO error mode to exception
                $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
                $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
                $sql = "select * from 店名 where SID = $SID";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                $row = $stmt->fetch();
                $shop_name = $row['店名'];
                $category = $row['類別'];
                $latitude = $row['緯度'];
                $longtitude = $row['經度'];
                echo '<div class="tab-content">
                        <div id="home" class="tab-pane fade in active">
                          <h3>Profile</h3>
                          <div class="row" display:inline>
                            <div class="col-xs-12">
                            <p nowrap>商店名稱:'.$shop_name.' 商店總類:'.$category.' 緯度: '.$latitude.' 經度: '.$longtitude.'
                            </div>
                          </div>
                        </div>
                      </div>';
              }
          ?>
        <div id="mea">
          <?php
                if(!$_SESSION['Authenticated']) //沒有商店
                {
                  echo '';
                }
                else //有商店
                {
                  echo '<h3>ADD</h3> 
                        <form action="add_meal.php" method="post" enctype="multipart/form-data">
                          <div class="form-group ">
                            <div class="row">  
                              <div class="col-xs-6">
                                <label for="ex3">meal name</label>
                                <input class="form-control" id="ex3" name="mealname" type="text">
                              </div>
                            </div>
                            <div class="row" style=" margin-top: 15px;">
                              <div class="col-xs-3">
                                <label for="ex7">price</label>
                                <input class="form-control" id="ex7" name="price" type="text">
                              </div>
                              <div class="col-xs-3">
                                <label for="ex4">quantity</label>
                                <input class="form-control" id="ex4" name="quantity" type="text">
                              </div>
                            </div>
                            <div class="row" style=" margin-top: 25px;">
                              <div class=" col-xs-3">
                                <label for="ex12">上傳圖片</label>
                                <input id="myFile" type="file" name="photo" multiple class="file-loading">
                              </div>
                              <div class=" col-xs-3">
                                <button style=" margin-top: 15px;" type="submit" value="add" class="btn btn-primary">Add</button>
                              </div>
                            </div>
                          </div>
                        </form>';
                }
          ?>
        </div>
        <div class="row" id = 'Null2'>
          <div class="  col-xs-8" id = "change">
            <?php
              $dbservername = 'localhost';
              $dbusername = "root";
              $dbpassword = "Ab1824018)";
              $dbname="hw2";
              $SID = $_SESSION['SID'];
              if(!$_SESSION['Authenticated']) //沒有商店
              {
                echo '';
              }
              else //有商店
              {
                $conn = new PDO("mysql:host=$dbservername;dbname=$dbname", 
                                  $dbusername, $dbpassword);
                                  # set the PDO error mode to exception
                $conn->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
                $conn->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION);
                $ans = '<table class="table" style=" margin-top: 15px;" >
                        <thead>
                          <tr>
                            <th scope="col">#</th>
                            <th scope="col">Picture</th>
                            <th scope="col">meal name</th>
                            <th scope="col">price</th>
                            <th scope="col">Quantity</th>
                            <th scope="col">Edit</th>
                            <th scope="col">Delete</th>
                          </tr>
                        </thead>
                        <tbody >
                        ';
                $sql = "select * from 商品 where SID = $SID";
                $stmt=$conn->prepare($sql);
                $stmt->execute();
                $row = $stmt->fetchALL();
                $i = 0;
                $cnt = 1;
                while($i<$stmt->rowCount())
                {
                  $type = $row[$i]['圖片型態'];
                  $picture = $row[$i]['圖片'];
                  $product_name = $row[$i]['商品名稱'];
                  $price = $row[$i]['價格'];
                  $number = $row[$i]['庫存'];
                  $ans = $ans."<tr>
                                <th scope=\"row\">".$cnt."</th>
                                <td > <img src=\"data:'".$type."';base64, ".$picture."\" /> </td>
                                <td >".$product_name."</td>
                                <td >".$price."</td>
                                <td >".$number."</td>
                                <td >
                                  <button type=\"button\" class=\"btn btn-info\" data-toggle=\"modal\" data-target=\"#".$product_name."\">Edit</button>
                                </td>
                                <td>
                                  <form action=\"delete.php\" class=\"fh5co-form animate-box\" data-animate-effect=\"fadeIn\" method = \"post\">
                                    <input name=\"mealname\" type=\"hidden\" value='".$product_name."'>
                                    <input type=\"submit\" value=\"Delete\" class=\"btn btn-primary\">
                                  </form>
                                </td>
                              </tr>";
                  $i += 1;
                  $cnt += 1;
                }
                $ans = $ans.' </tbody>
                            </table>';
                $i = 0;
                while($i<$stmt->rowCount())
                {
                  $product_name = $row[$i]['商品名稱'];
                  $ans = $ans.'<div class="modal fade" id="'.$product_name.'" data-backdrop="static" tabindex="-1" role="dialog" aria-labelledby="staticBackdropLabel" aria-hidden="true">
                                <div class="modal-dialog" role="document">
                                  <div class="modal-content">
                                    <div class="modal-header">
                                      <h5 class="modal-title" id="staticBackdropLabel">coffee Edit</h5>
                                      <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                                        <span aria-hidden="true">&times;</span>
                                      </button>
                                    </div>
                                    <form action="update.php" class="fh5co-form animate-box" data-animate-effect="fadeIn" method = "post">
                                      <div class="modal-body">
                                        <div class="row" >
                                          <div class="col-xs-6">
                                            <div class="form-group">
                                              <label for="ex72">price</label>
                                              <input class="form-control" id="ex72" type="text" name = "price">
                                            </div>
                                          </div>
                                          <div class="col-xs-6">
                                            <div class="form-group">
                                              <label for="ex42">quantity</label>
                                              <input class="form-control" id="ex42" type="text" name = "quantity">
                                            </div>
                                          </div>
                                          <div class="form-group">
                                            <input name="mealname" type="hidden" value="'.$product_name.'">
                                          </div>
                                        </div>
                                      </div>
                                      <div class="modal-footer">
                                        <button type="input" class="btn btn-primary" value="Edit">Edit</input>
                                      </div>
                                    </form>
                                  </div>
                                </div>
                              </div>';
                  $i += 1;
                }
                echo $ans;
              }
            ?>
          </div>
        </div>
    </div>

    <!-- Myorder -->
    <div id="menu2" class="tab-pane fade" >
      <div class=" row  col-xs-8">
          <br></br>
            <form class="form-horizontal" >
              <div class="form-group">
                <label class="control-label col-sm-1" for="distance">Status</label>
                <div class="col-sm-5">
                    <select class="form-control" name="option" oninput="showresult_my_order(this.value)">
                      <option selected>All</option>
                      <option>Finished</option>
                      <option>Not Finish</option>
                      <option>Cancel</option>
                    </select>
                </div>
              </div>
            </form>
            <form  action="cancel.php" class="fh5co-form animate-box" data-animate-effect="fadeIn" method = "post">
              <table class="table" style=" margin-top: 15px;">
                <thead>
                    <tr>
                      <th scope="col">Order ID</th>
                      <th scope="col">Status</th>
                      <th scope="col">Start</th>
                      <th scope="col">End</th>
                      <th scope="col">Shop Name</th>
                      <th scope="col">Total Price</th>
                      <th scope="col">Order Details</th>
                      <th scope="col">Action</th>
                    </tr>
                </thead>
                <tbody id='filt_my_order'>

                </tbody>
              </table>
              
            </form>
            <div id="detail_my_order">

            </div>
      </div>
    </div>
    <!-- shop order -->
    <div id="menu3" class="tab-pane fade" >
    ` <div class=" row  col-xs-8">
          <br></br>
            <form class="form-horizontal" action="shop_order.php" method="post">
              <div class="form-group">
                <label class="control-label col-sm-1" for="distance">Status</label>
                <div class="col-sm-5">
                    <select class="form-control" name="option" oninput="showresult_shop_order(this.value)">
                      <option selected>All</option>
                      <option>Finished</option>
                      <option>Not Finish</option>
                      <option>Cancel</option>
                    </select>
                </div>
              </div>
            </form>
            <form  action="cancel_shop.php" class="fh5co-form animate-box" data-animate-effect="fadeIn" method = "post">
              <table class="table" style=" margin-top: 15px;">
                <thead>
                    <tr>
                      <th scope="col">Order ID</th>
                      <th scope="col">Status</th>
                      <th scope="col">Start</th>
                      <th scope="col">End</th>
                      <th scope="col">Shop Name</th>
                      <th scope="col">Total Price</th>
                      <th scope="col">Order Details</th>
                      <th scope="col">Action</th>
                    </tr>
                </thead>
                <tbody id="filt_shop_order">
                  
                </tbody>
              </table>
                
            </form>
            <div id='detail_shop_order'>
            </div>
      </div>
    </div>
    <!-- transaction -->
    <div id="menu4" class="tab-pane fade" >
        <div class=" row  col-xs-8">
          <br></br>
            <form class="form-horizontal">
              <div class="form-group">
                <label class="control-label col-sm-1" for="distance"> Action</label>
                <div class="col-sm-5">
                    <select class="form-control"  oninput="showresult_transaction(this.value)">
                      <option selected>All</option>
                      <option>付款 </option>
                      <option>收款</option>
                      <option>儲值</option>
                    </select>
                </div>
              </div>
            </form>
            <table class="table" style=" margin-top: 15px;"><thead>
                <tr>
                  <th scope="col">Record ID</th>
                  <th scope="col">Action</th>
                  <th scope="col">Time</th>
                  <th scope="col">Trader</th>
                  <th scope="col">Amount change</th>
                </tr>
              </thead>
              <tbody id='filt_transaction'>

              </tbody>
            </table>
        </div>
    </div>
    </div>
   
      



    </div>
  </div>

  <!-- Option 1: Bootstrap Bundle with Popper -->
  <!-- <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p" crossorigin="anonymous"></script> -->
  <script>
    $(document).ready(function () {
      $(".nav-tabs a").click(function () {
        $(this).tab('show');
      });
    });
  </script>

  <!-- Option 2: Separate Popper and Bootstrap JS -->
  <!--
    <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.10.2/dist/umd/popper.min.js" integrity="sha384-7+zCNj/IqJ95wo16oMtfsKbZ9ccEh31eOz1HGyDuCQ6wgnyJNSYdrPa03rtR1zdB" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.min.js" integrity="sha384-QJHtvGhmr9XOIpI6YVutG+2QOK9T+ZnN4kzFN1RtK3zEFEIsxhlmWl5/YESvpZ13" crossorigin="anonymous"></script>
    -->
</body>

</html>