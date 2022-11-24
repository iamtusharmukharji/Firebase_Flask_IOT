from flask import Flask, request
from flask_mqtt import Mqtt
from datetime import datetime
import models


apis = Flask(__name__)

apis.config['MQTT_BROKER_URL'] = 'test.mosquitto.org'
apis.config['MQTT_BROKER_PORT'] = 1883
apis.config['MQTT_USERNAME'] = ''
apis.config['MQTT_PASSWORD'] = ''
apis.config['MQTT_REFRESH_TIME'] = 1.0

mqtt = Mqtt(apis)

myTopic= 'xx342/control'

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    mqtt.subscribe(myTopic)
    print("MQTT Connected")


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    
    payload=message.payload.decode()
    print(f"Message Recieved: {message.topic} -> {payload}")
    return {"status":"state_changed", "data":payload}


@apis.route("/")
def home():
    return "API server is running.."

# to control a button
@apis.route('/publish/',methods=["PUT"])
def publish(db=models.start_session()):

    try:
        payload = request.get_json()

        pin = payload['pin']
        state = payload['state']
        publish_data = f'"{pin}-{state}"'
        
        mqtt.publish(myTopic, str(publish_data))

        check_button = db.query(models.Button).filter(models.Button.button_tag == pin).first()
        print("Check Button: ",check_button)

        if check_button == None:
            new_button = models.Button(button_tag = pin, 
                                        state = state,
                                        last_update = datetime.now()
                                        )
            db.add(new_button)
        else:
            check_button.state = state
            check_button.last_update = datetime.now()
        
        db.commit()
        db.close()

        return {"button_id":pin, "state":state}, 201
    
    except Exception as err:
        return {"detail":str(err)}, 400





# to get the button states
@apis.route("/button/state/")
def button_state(db=models.start_session()):
    try:
        buttons = db.query(models.Button).all()
        res = []
        if buttons == []:
            db.close()
            return {"detail":"no buttons found"}, 404
        
        for butt in buttons:
            temp={}
            temp["button_id"] = butt.button_tag
            temp["state"] = butt.state
            temp["last_updated"] = butt.last_update
            res.append(temp)
        db.close()
        return {"data":res}, 200


    except Exception as err:
        return {"detail":str(err)}, 400




if __name__ == "__main__":
    apis.run(debug=True)
