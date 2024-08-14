#!/home/lu/anaconda3/bin/python3
import cv2
import datetime
from ultralytics import YOLO
import cv2
import base64
# Open the first available camera
print("hello")
'''cap = cv2.VideoCapture(0)

# Check if the camera is opened successfully
if not cap.isOpened():
    print("Failed to open the camera")
    exit()
model = YOLO("yolov8n.pt")
# Read and display frames from the camera
CONFIDENCE_THRESHOLD = 0.5
GREEN = (0, 255, 0)
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
    # end time to compute the fps
    end = datetime.datetime.now()
    # show the time it took to process 1 frame
    total = (end - start).total_seconds()
    print(f"Time to process 1 frame: {total * 1000:.0f} milliseconds")

    # calculate the frame per second and draw it on the frame
    fps = f"FPS: {1 / total:.2f}"
    cv2.putText(frame, fps, (50, 50),
                cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 0, 255), 8)
    base64_image = base64.b64encode(frame).decode('utf-8')
    print(base64_image)
    # Press 'q' to quit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera and close windows
cap.release()
cv2.destroyAllWindows()'''