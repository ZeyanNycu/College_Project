# DAI2.py #coding=utf-8 -- new version of Dummy Device DAI.py, modified by tsaiwn@cs.nctu.edu.tw
import time, DAN, requests, random 
import threading, sys # for using a Thread to read keyboard INPUT
import matplotlib.pyplot as plt
import os
import re
Bulb_sign = True
Buzzer_sign = True
recipient_email = "braque71561@gmail.com"
threshold=930
over = False
have_people = False
def line_box():
   #Python module requirement: line-bot-sdk, flask
   from flask import Flask, request, abort
   from linebot import LineBotApi, WebhookHandler
   from linebot.exceptions import InvalidSignatureError 
   from linebot.models import MessageEvent, TextMessage, TextSendMessage
   import DAN
   import random
   line_bot_api = LineBotApi('4KKk3Ntf5YJgStSo6E+XvUJiWmaqfYsY1rOCLHn0HCQiSop2oMfQPWBeUkzD++uZ1kb0GDWhgFX0ad+SF8/dzk65giSHI4ivra/UvXTnqDiXq/oG54zVQCg/YTQXxj3gol+q389zWwKXuGeEoPanKwdB04t89/1O/w1cDnyilFU=') #LineBot's Channel access token
   handler = WebhookHandler('dc1d18401523d789a32a68aceba3e16f')        #LineBot's Channel secret
   user_id_set=set()                                         #LineBot's Friend's user id 
   app = Flask(__name__)
   def loadUserId():
      try:
         idFile = open('idfile', 'r')
         idList = idFile.readlines()
         idFile.close()
         idList = idList[0].split(';')
         idList.pop()
         return idList
      except Exception as e:
         print(e)
         return None
   def saveUserId(userId):
      idFile = open('idfile', 'a')
      idFile.write(userId+';')
      idFile.close()


   @app.route("/", methods=['GET'])
   def hello():
      return "HTTPS Test OK."

   @app.route("/", methods=['POST'])
   def callback():
      signature = request.headers['X-Line-Signature']    # get X-Line-Signature header value
      body = request.get_data(as_text=True)              # get request body as text
      #print("Request body: " + body, "Signature: " + signature)
      try:
         handler.handle(body, signature)                # handle webhook body
      except InvalidSignatureError:
         abort(400)
      return 'OK'


   @handler.add(MessageEvent, message=TextMessage)
   def handle_message(event):
      Msg = event.message.text
      if Msg == 'Hello, world': return
      print('GotMsg:{}'.format(Msg))
      control_msg_time_interval = re.findall("msg time:[0-9]+",Msg)
      if(len(control_msg_time_interval)):
         global m_interval
         control_msg_time_interval = control_msg_time_interval[0].split(":")[1]
         control_msg_time_interval = int(control_msg_time_interval)
         m_interval = control_msg_time_interval
         s = "Change the interval of message to "+ str(m_interval) + "s"
         line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
      control_threshold = re.findall("threshold:[0-9]+",Msg)
      if(len(control_threshold)):
         global threshold
         control_threshold = control_threshold[0].split(":")[1]
         control_threshold = int(control_threshold)
         threshold = control_threshold
         s = "Change the threshold to "+ str(threshold)
         line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
      control_email = re.findall("email time:[0-9]+",Msg)
      if(len(control_email)):
         global e_interval
         control_email = control_email[0].split(":")[1]
         control_email = int(control_email)
         e_interval = control_email
         s = "Change the interval of email to "+ str(e_interval) + "s"
         line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
      control_Bulb = re.findall("Bulb:[a-z]+",Msg)
      if(len(control_email)):
         global Bulb_sign
         control_Bulb = control_Bulb[0].split(":")[1]
         if(control_Bulb == "on"):
            Bulb_sign = True
            s = "Open the Bulb"
            line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
         elif(control_Bulb == "off"):
            Bulb_sign = False
            s = "Close the Bulb"
            line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
         else:
            s = "Unknown cmd"
            line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
      control_Buzzer = re.findall("Buzzer:[a-z]+",Msg)
      if(len(control_Buzzer)):
         global Buzzer_sign
         control_Buzzer = control_Buzzer[0].split(":")[1]
         if(control_Buzzer == "on"):
            Buzzer_sign = True
            s = "Open the Buzzer"
            line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
         elif(control_Buzzer == "off"):
            Buzzer_sign = False
            s = "Close the Buzzer"
            line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
         else:
            s = "Unknown cmd"
            line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
      state = re.findall("state",Msg)
      if(len(state)):
         s = "Message interval: "+str(m_interval) +"\n"
         s += "Email interval: "+str(e_interval) +"\n"
         s += "Threshold: "+str(threshold) +"\n"
         s += "Bulb: "+str(Bulb_sign)+"\n"
         s += "Buzzer: "+str(Buzzer_sign)
         line_bot_api.reply_message(event.reply_token,TextSendMessage(text=s))
      userId = event.source.user_id
      if not userId in user_id_set:
         user_id_set.add(userId)
         saveUserId(userId)
   app.run('127.0.0.1', port=32768, threaded=True, use_reloader=False)
def write_to_line_box(s):
   #Python module requirement: line-bot-sdk, flask
   from flask import Flask, request, abort
   from linebot import LineBotApi, WebhookHandler
   from linebot.exceptions import InvalidSignatureError 
   from linebot.models import MessageEvent, TextMessage, TextSendMessage
   import DAN
   import random
   line_bot_api = LineBotApi('4KKk3Ntf5YJgStSo6E+XvUJiWmaqfYsY1rOCLHn0HCQiSop2oMfQPWBeUkzD++uZ1kb0GDWhgFX0ad+SF8/dzk65giSHI4ivra/UvXTnqDiXq/oG54zVQCg/YTQXxj3gol+q389zWwKXuGeEoPanKwdB04t89/1O/w1cDnyilFU=') #LineBot's Channel access token
   handler = WebhookHandler('dc1d18401523d789a32a68aceba3e16f')        #LineBot's Channel secret
   user_id_set=set()
   def loadUserId():
      try:
         idFile = open('idfile', 'r')
         idList = idFile.readlines()
         idFile.close()
         idList = idList[0].split(';')
         idList.pop()
         return idList
      except Exception as e:
         print(e)
         return None
   def saveUserId(userId):
      idFile = open('idfile', 'a')
      idFile.write(userId+';')
      idFile.close()
   idList = loadUserId()
   if idList: user_id_set = set(idList)
   try:
      for userId in user_id_set:
         line_bot_api.push_message(userId, TextSendMessage(text=s))  # Push API example
   except Exception as e:
      print(e)
def web_server():
   #Python module requirement: line-bot-sdk, flask
   from flask import Flask, request, abort,render_template,redirect
   from linebot import LineBotApi, WebhookHandler
   from linebot.exceptions import InvalidSignatureError 
   from linebot.models import MessageEvent, TextMessage, TextSendMessage
   from flask_httpauth import HTTPBasicAuth
   from werkzeug.security import generate_password_hash, check_password_hash
   import random
   import netifaces 
   app = Flask(__name__)
   auth = HTTPBasicAuth()

   users = {
      "IOT": generate_password_hash("final"),
      "susan": generate_password_hash("bye")
   }

   @auth.verify_password
   def verify_password(username, password):
      f = open('password')
      if username in users and check_password_hash(f.read(), password):
         f.close()
         return username
      f.close()
   @app.route('/setting_password',methods=['POST','GET'])
   @auth.login_required
   def setting_password():
      o_pass = request.form.get('o_pass')
      n_pass1 = request.form.get('n_pass1')
      n_pass2 = request.form.get('n_pass2')
      if(n_pass1 != None and n_pass2 != None):
         if(n_pass1==n_pass2):
            password_file = open('password','r')
            s = password_file.read()
            if (check_password_hash(s,o_pass)):
               password_file.close()
               f = open('password','w')
               f.write(generate_password_hash(n_pass1))
               f.close()
               print('Change Password Successfully')
            else:
               password_file.close()
      return redirect("http://"+ipv4_address+":"+"8888/setting", code=302)
   @app.route('/setting_email',methods=['POST','GET'])
   @auth.login_required
   def setting_email():
      global recipient_email
      mail = request.form.get('email')
      if(mail != None):
         recipient_email = mail
      return redirect("http://"+ipv4_address+":"+"8888/setting", code=302)
   @app.route('/setting',methods=['POST','GET'])
   @auth.login_required
   def setting():
      return render_template('setting.php')
   @app.route('/',methods=['POST','GET'])
   @auth.login_required
   def index():
      if(have_people and over):
         return render_template('web1.php')
      return render_template('web.php')
   @app.route('/submit',methods=['POST','GET'])
   @auth.login_required
   def submit():
      global m_interval
      global e_interval
      global threshold
      mesg_time = request.form.get('value1')
      email_time = request.form.get('value2')
      temp_th = request.form.get('value3')
      try:
         m_interval = int(mesg_time)
      except:
         m_interval = 60
      try:
         e_interval = int(email_time)
      except:
         e_interval = 60
      try:
         threshold = int(temp_th)
      except:
         threshold = 930
      return redirect("http://"+ipv4_address+":"+"8888", code=302)
   @app.route('/submit1',methods=['POST','GET'])
   @auth.login_required
   def submit1():
      global Bulb_sign
      global Buzzer_sign
      temp_Bulb_sign = request.form.get('Bulb')
      temp_Buzzer_sign = request.form.get('Buzzer')
      try:
         if(temp_Bulb_sign == "on"):
            Bulb_sign = True
         else:
            Bulb_sign = False
      except:
         Bulb_sign = False
      try:
         if(temp_Buzzer_sign == "on"):
            Buzzer_sign = True
         else:
            Buzzer_sign = False
      except:
         Buzzer_sign = False
      return redirect("http://"+ipv4_address+":"+"8888", code=302)
   #if_addresses = netifaces.ifaddresses("enx5c925ed2a559")
   if_addresses = netifaces.ifaddresses("wlo1")
   ipv4_address = if_addresses[netifaces.AF_INET][0]['addr']

   # Print the IPv4 address
   print("IPv4 Address:", ipv4_address)
   app.run(ipv4_address , port=8888, threaded=True, use_reloader=False)
def write_to_email(s):
   import smtplib,ssl
   from email.mime.text import MIMEText
   from email.mime.multipart import MIMEMultipart
   global recipient_emai
   # Sender's email credentials
   sender_email = "ding.kennyss@gmail.com"
   sender_password = "uwwagcscjxprhssn"

   # Recipient's email address
   

   # Create a multipart message
   message = MIMEMultipart()
   message["From"] = sender_email
   message["To"] = recipient_email
   message["Subject"] = "Example Email Subject"

   # Add body text to the email
   message.attach(MIMEText(s, "plain"))

   # SMTP server configuration
   smtp_server = "smtp.gmail.com"
   smtp_port = 587
   context=ssl.create_default_context()
   try:
      # Create a secure connection to the SMTP server
      server = smtplib.SMTP(smtp_server,587)
      server.ehlo()
      server.starttls()
      server.login(sender_email, sender_password)
      # Send the email
      server.sendmail(sender_email, recipient_email, message.as_string())
      print("Email sent successfully!")
      
   except Exception as e:
      print("An error occurred while sending the email:", str(e))
   finally:
      server.quit()
def open_camera():
   import cv2
   import datetime
   from ultralytics import YOLO
   import cv2
   import time
   # Open the first available camera
   cap = cv2.VideoCapture(0)
   global have_people
   # Check if the camera is opened successfully
   if not cap.isOpened():
      print("Failed to open the camera")
      exit()
   model = YOLO("yolov8n.pt")
   # Read and display frames from the camera
   CONFIDENCE_THRESHOLD = 0.5
   GREEN = (0, 255, 0)
   have_people = False
   while True:
      start = datetime.datetime.now()

      ret, frame = cap.read()
      
      if not ret:
         print("Failed to capture frame")
         break
      detections = model(frame)[0]
      for data in detections.boxes.data.tolist():
         # extract the confidence (i.e., probability) associated with the detection
         confidence = data[4]

         # filter out weak detections by ensuring the 
         # confidence is greater than the minimum confidence
         if float(confidence) < CONFIDENCE_THRESHOLD:
               continue

         # if the confidence is greater than the minimum confidence,
         # draw the bounding box on the frame
         xmin, ymin, xmax, ymax = int(data[0]), int(data[1]), int(data[2]), int(data[3])
         cv2.rectangle(frame, (xmin, ymin) , (xmax, ymax), GREEN, 2)
         have_people = True
      # end time to compute the fps
      end = datetime.datetime.now()
      # show the time it took to process 1 frame
      total = (end - start).total_seconds()

      # calculate the frame per second and draw it on the frame
      fps = f"FPS: {1 / total:.2f}"
      cv2.putText(frame, fps, (50, 50),
                  cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 0, 255), 8)
      cv2.imwrite("static/camera.jpeg",frame)
      time.sleep(1)

   # Release the camera and close windows
   cap.release()
   cv2.destroyAllWindows()
# ServerURL = 'http://Your_server_IP_or_DomainName:9999' #with no secure connection
#  注意你用的 IoTtalk 伺服器網址或 IP  #  https://goo.gl/6jtP41
ServerURL = 'https://5.iottalk.tw' # with SSL secure connection
# ServerURL = 'https://Your_DomainName' #with SSL connection  (IP can not be used with https)
Reg_addr = None #if None, Reg_addr = MAC address #(本來在 DAN.py 要這樣做 :-) 
# Note that Reg_addr 在以下三句會被換掉! # the mac_addr in DAN.py is NOT used
mac_addr = 'CD8600D38' + str( 955 )  # put here for easy to modify :-)
# 若希望每次執行這程式都被認為同一個 Dummy_Device, 要把上列 mac_addr 寫死, 不要用亂數。
Reg_addr = mac_addr   # Note that the mac_addr generated in DAN.py always be the same cause using UUID !
DAN.profile['dm_name']='Dummy_Device'   # you can change this but should also add the DM in server
DAN.profile['df_list']=['Dummy_Sensor', 'Dummy_Control']   # Check IoTtalk to see what IDF/ODF the DM has
DAN.profile['d_name']= "TWN_D071."+ "955" +"_"+ DAN.profile['dm_name'] # None
DAN.device_registration_with_retry(ServerURL, Reg_addr) 
print("dm_name is ", DAN.profile['dm_name']) ; print("Server is ", ServerURL);
# global gotInput, theInput, allDead    ## 主程式不必宣告 globel, 但寫了也 OK
gotInput=False
theInput="haha"
allDead=False

#creat a thread to do Input data from keyboard, by tsaiwn@cs.nctu.edu.tw 
threadx = threading.Thread(target=line_box)
threadx.daemon = True  # 這樣才不會阻礙到主程式的結束
threadx.start()
thread_web = threading.Thread(target=web_server)
thread_web.daemon = True  # 這樣才不會阻礙到主程式的結束
thread_web.start()
thread_camera = threading.Thread(target=open_camera)
thread_camera.daemon = True  # 這樣才不會阻礙到主程式的結束
thread_camera.start()
line_timestamp = []
email_timestamp = []
smoke_value = []
m_interval = 60
e_interval = 60
while True:
    try:
      if(allDead): break;
      #Pull data from a device feature called "Dummy_Control"
      value1=DAN.pull('Dummy_Control')
      control = [0,0]
      if value1 != None:    # 不等於 None 表示有抓到資料
         #print (value1[0])#The type of the class is int
         if os.path.exists('static/data.png'):
            os.remove('static/data.png')
         smoke_value.append([value1[0],threshold])
         for i in range(len(smoke_value)):
            smoke_value[i][1] = threshold
         if(len(smoke_value)>50):
            smoke_value = smoke_value[-50:]
         plt.plot(smoke_value)
         plt.savefig('static/data.png')
         plt.clf()
         plt.close()
         if(value1[0]>threshold):
            over = True
            #write the application that can send to line box
            cur_time = time.time()
            if(len(line_timestamp) == 0 or cur_time-line_timestamp[-1]>m_interval):
               write_to_line_box('Smoke concentration exceeding the average value.')
               line_timestamp.append(cur_time)
            if(len(email_timestamp) == 0 or cur_time-email_timestamp[-1]>e_interval):
               write_to_email('Smoke concentration exceeding the average value.')
               email_timestamp.append(cur_time)
            if(Bulb_sign):
               control[0] = 1
            if(Buzzer_sign):
               control[1] = 2
            DAN.push ('Dummy_Sensor', control)  #  試這:  DAN.push('Dummy_Sensor', theInput) 
         else:
            over=False

    except Exception as e:
        print(e)
        if str(e).find('mac_addr not found:') != -1:
            print('Reg_addr is not found. Try to re-register...')
            DAN.device_registration_with_retry(ServerURL, Reg_addr)
        else:
            print('Connection failed due to unknow reasons.')
            time.sleep(1)    
    try:
       time.sleep(0.2)
    except KeyboardInterrupt:
       break
time.sleep(0.25)
try: 
   DAN.deregister()    # 試著解除註冊
except Exception as e:
   print("===")
print("Bye ! --------------", flush=True)
sys.exit(0);