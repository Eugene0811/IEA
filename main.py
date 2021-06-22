import cv2
import numpy as np
import face_recognition
import os
import paho.mqtt.client as mqtt
import random as rd
import string

hostname = "test.mosquitto.org"
RoomName = "TNI-Robot/ntt"
ConnectionStatus = False
global DummyVar
DummyVar = "\n"
port = 1883


def on_connection(client, user_data, flag, rc):
    global conn_status  # global variable in this file
    status_decoder = {  # swtich case in python style using dictionary
        0: "Successfully Connected",
        1: "Connection refused: Incorrect Protocol Version",
        2: "Connection refused: Invalid Client Identifier",
        3: "Connection refused: Server Unavailable",
        4: "Connection refused: Bad Username/Password",
        5: "Connection refused: Not Authorized",
    }
    print("System has been connected with status", status_decoder.get(rc))
    conn_status = True


def on_message(client, user_data, msg):
    # check incoming payload to prevent owner echo text
    global incoming_massage
    incoming_massage = msg.payload.decode("utf-8")
    if incoming_massage.find(DummyVar) >= 0:
        pass
    else:
        print(incoming_massage)


def send_request(name):
    message = name + "\n"
    client.publish(RoomName, message)


path = 'ImageAttendance'
familyImages = []  # All dataset images
familyList = []  # All dataset info
familyNames = []  # Dataset names
familyId = []  # Dataset Ids
myList = os.listdir(path)

for i in myList:
    curImg = cv2.imread(f'{path}/{i}')
    familyImages.append(curImg)
    familyList.append(os.path.splitext(i)[0])
for i in familyList:
    familyId.append(i.split('_')[0])
    familyNames.append(i.split('_')[1])


def findEncodings(images):  # Use to encode and keep results in a list
    encodeList = []
    index = 0
    for eachImage in images:
        eachImage = cv2.cvtColor(eachImage, cv2.COLOR_BGR2RGB)
        encode = face_recognition.face_encodings(eachImage)[0]
        encodeList.append(encode)
        print('Encoding... index = ' + str(index))
        index += 1
    return encodeList


client_id = 'Client-' + ''.join(rd.choices(string.ascii_uppercase + string.digits, k=9))
client = mqtt.Client(client_id)
client.on_connect = on_connection
client.on_message = on_message
client.connect(hostname, port)
client.loop_start()

encodeListKnown = findEncodings(familyImages)  # List of encoded familyImages
print('Encoding Complete')

status = False
pressKeyVideo = ""
pressKeyImage = ""

cap = cv2.VideoCapture(0)
while pressKeyVideo != 27:
    success, rawImage = cap.read()
    capturedImage = cv2.resize(rawImage, (0, 0), None, 0.25, 0.25)
    capturedImage = cv2.cvtColor(capturedImage, cv2.COLOR_BGR2RGB)

    faceCurFrame = face_recognition.face_locations(capturedImage)
    encodeCurFrame = face_recognition.face_encodings(capturedImage, faceCurFrame)

    for encodeFace, faceLoc in zip(encodeCurFrame, faceCurFrame):
        matches = face_recognition.compare_faces(encodeListKnown, encodeFace)
        faceDis = face_recognition.face_distance(encodeListKnown, encodeFace)

        matchIndex = np.argmin(faceDis)
        print(str(faceDis[matchIndex]))

        y1, x2, y2, x1 = faceLoc
        y1, x2, y2, x1 = y1 * 4, x2 * 4, y2 * 4, x1 * 4
        cv2.rectangle(rawImage, (x1, y1), (x2, y2), (150, 10, 255), 2)
        cv2.rectangle(rawImage, (x1, y2 - 20), (x2, y2), (150, 10, 255), cv2.FILLED)

        if faceDis[matchIndex] < 0.4:
            id = int(familyId[matchIndex])
            name = str(familyNames[matchIndex])
            print("Id = " + str(id) + " & Name = " + str(name))

            if name == "Teeraphat Kullanankanjana":
                send_request(name)
                print("Hello World!\n")

            if name == "Chayakotchamon Boonsanong":
                send_request(name)
                print("Owen\n")

            if name == "Thanaporn Sanamontre":
                send_request(name)
                print("Opaaaaaaaaaaaaaaaaaaaaa\n")

            status = True
            break

    # if status is True:
    #     final, finalImage = cap.read()
    #     break

    cv2.imshow('Video', rawImage)
    pressKeyVideo = cv2.waitKey(10)

# while pressKeyImage != 27:
#     cv2.imshow('Image', finalImage)
#     pressKeyImage = cv2.waitKey(10)

cap.release()
cv2.destroyAllWindows()
