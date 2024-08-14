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
        <a href=http://140.113.121.214:8888> Manage </a>
        <h1 style="text-align:center"><strong>Setting</strong></h1>
        <form action="/setting_email" method="POST">
            <table style="height:200px; width:1210px; border:1px solid black;margin-left:auto;margin-right:auto;">
                <tbody>
                    <tr>
                        <th style="text-align:center;width:1191px;border-bottom: 1px solid black">Personal Change</th>
                    </tr>
                    <tr>
                        <td style="text-align:center">Email: <input type="text" name="email" ></td>
                    </tr>
                </tbody>
            </table>
            <div class="container">  
                <input type="submit"  name="submit" value="submit" />
            </div>
        </form>
        <form action="/setting_password" method="POST">
            <table style="height:300px; width:1210px; border:1px solid black;margin-left:auto;margin-right:auto;">
                <tbody>
                    <tr>
                        
                        <th style="text-align:center;width:1191px;border-bottom: 1px solid black">Personal Change</th>
                    </tr>
                    <tr>
                        <td style="text-align:center">Original Password: <input type="text" name="o_pass" ></td>
                    </tr>
                    <tr>
                        <td style="text-align:center">New Password: <input type="text" name="n_pass1" ></td>
                    </tr>
                    <tr>
                        <td style="text-align:center">New Password(Again): <input type="text" name="n_pass2" ></td>
                    </tr>
                </tbody>
            </table>
            <div class="container">  
                <input type="submit"  name="submit" value="submit" />
            </div>
        </form>
    </body>
</html>
