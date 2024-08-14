<!DOCTYPE html>
<html>
    <head>
        <title>IOT Demo</title>
    </head>
    <style>  
        .container{  
            text-align: center;    
            width: 1820px;  
            height: 100px;  
            padding-top: 20px;  
        }  
        #btn{  
            font-size: 25px;  
        }
        .toggle-switch-contrainer{
            text-align: center;    
            width: 1200px;  
            height: 0px;  
            padding-top: 0px;
        }
        .toggle-switch-contrainer1{
            text-align: center;    
            width: 2450px;  
            height: 34px;  
            padding-top: 0px;
        }
        .toggle-switch {
            position: relative;
            display: inline-block;
            width: 60px;
            height: 34px;
        }

        .toggle-switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }

        .toggle-slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: gray;
            transition: .4s;
        }

        .toggle-slider:before {
            position: absolute;
            content: "";
            height: 26px;
            width: 26px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
        }

        input:checked + .toggle-slider {
            background-color: green;
        }

        input:checked + .toggle-slider:before {
            transform: translateX(26px);
        }
    </style> 
    <body>
        <a href=http://140.113.121.214:8888/setting> Setting </a>
        <h1 style="text-align:center"><strong>Smoke and haze detection values table</strong></h1>
        <table style="height:0px; width:1210px;margin-left:auto;margin-right:auto;">
                <tbody>
                    <tr>
                        <th style="text-align:center;width:591px;border-bottom: 1px solid black"><p style="text-align:center"><strong><img alt="" src="/static/data.png" style="height:350px; width:666px" /></strong></p></th>
                        <th style="text-align:center;width:591px;border-bottom: 1px solid black"><p style="text-align:center"><strong><img alt="" src="/static/camera.jpeg" style="height:350px; width:666px" /></strong></p></th>
                    </tr>
                </tbody>
        </table>

        <h1 style="text-align:center"><strong>The Control panel of the Board</strong></h1>
        <form action="/submit" method="POST">
            <table style="height:115px; width:1210px; border:1px solid black;margin-left:auto;margin-right:auto;">
                <tbody>
                    <tr>
                        <th style="text-align:center;width:591px;border-bottom: 1px solid black">Message time interval</th>
                        <th style="text-align:center;width:603px;border-bottom: 1px solid black;border-left: 1px solid black;">Threshold</th>
                        <th style="text-align:center;width:603px;border-bottom: 1px solid black;border-left: 1px solid black;">Email time interval</th>
                    </tr>
                    <tr>
                        <td style="text-align:center"><input type="text" name="value1" ></td>
                        <td style="text-align:center;border-left: 1px solid black;"><input type="text" name="value3" ></td>
                        <td style="text-align:center;border-left: 1px solid black;"><input type="text" name="value2" ></td>
                    </tr>
                </tbody>
            </table>
            <div class="container">  
                <input type="submit"  name="submit" value="submit" />
            </div>
        </form>
        <form action="/submit1" method="POST">
            <table style="height:115px; width:1210px; margin-left:auto;margin-right:auto;">
                <tbody>
                    <tr>
                        <th style="text-align:center;width:591px;">Bulb</th>
                        <th style="text-align:center;width:603px;">Buzzer</th>
                    </tr>
                    <tr>
                        <td style="text-align:center">
                            <label class="toggle-switch" >
                                <input type="checkbox" name="Bulb" checked>
                                <span class="toggle-slider"></span>
                            </label>
                        </td>
                        <td style="text-align:center;">
                            <label class="toggle-switch" >
                                    <input type="checkbox" name="Buzzer" checked>
                                    <span class="toggle-slider"></span>
                            </label>
                        </td>
                    </tr>
                </tbody>
            </table>
            <div class="container">  
                <input type="submit"  name="submit" value="submit" />
            </div>
        </form>
        
            
        
    </body>
</html>
