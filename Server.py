import paho.mqtt.client as mqtt
import threading
import signal
import cv2
import numpy as np
import time
import sys
import os

os.environ["QT_QPA_PLATFORM"] = "xcb"


class RobotControlSystem:
    def __init__(self):
        self.broker_ip = "localhost"
        self.port = 1883
        self.laptop_topic = "laptop/commands"
        self.robot_topic = "robot/gpio"
        self.conveyor_topic = "conveyor/commands"

        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

        self.cap = self.initialize_camera()
        self.window_name = "Robot Vision Control"
        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(self.window_name, 800, 600)

        self.color_ranges = {
            'red': (np.array([0, 120, 70]), np.array([10, 255, 255])),
            'blue': (np.array([90, 120, 70]), np.array([130, 255, 255])),
            'green': (np.array([40, 120, 70]), np.array([80, 255, 255])),
            'yellow': (np.array([20, 100, 100]), np.array([40, 255, 255])),
            'white': (np.array([0, 0, 200]), np.array([180, 30, 255]))
        }

        self.forward_sequence = ['green', 'yellow', 'white']
        self.backward_sequence = ['yellow', 'green']
        self.current_sequence = self.forward_sequence
        self.current_marker_index = 0
        self.mission_active = False
        self.loading_complete = False

        self.running = False
        self.vision_enabled = False
        self.show_camera = True
        self.front = (-1, -1)
        self.back = (-1, -1)
        self.target = (-1, -1)

    def initialize_camera(self):
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            raise RuntimeError("Failed to open camera")
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        cap.set(cv2.CAP_PROP_FPS, 30)
        return cap

    def on_connect(self, client, userdata, flags, reason_code, properties):
        print(f"Connected to MQTT broker at {self.broker_ip}")
        client.subscribe(self.laptop_topic)
        client.subscribe(self.conveyor_topic)

    def on_message(self, client, userdata, msg):
        try:
            payload = msg.payload.decode('utf-8').lower()
            if not payload:
                print("Empty message received")
                return

            if payload == "shutdown":
                self.shutdown()
            elif payload == "start_vision":
                self.vision_enabled = True
                print("Autonomous vision mode activated")
            elif payload == "stop_vision":
                self.vision_enabled = False
                print("Manual control mode activated")
            elif payload == "toggle_camera":
                self.show_camera = not self.show_camera
                print(f"Camera display {'enabled' if self.show_camera else 'disabled'}")
            elif payload == "start_mission":
                self.start_mission()
            elif payload == "loading_complete":
                self.loading_complete = True
            else:
                self.send_gpio_command(payload)

        except Exception as e:
            print(f"Error processing message: {str(e)}")

    def start_mission(self):
        """Начать миссию следования по меткам"""
        if not self.mission_active:
            self.mission_active = True
            self.current_sequence = self.forward_sequence
            self.current_marker_index = 0
            self.loading_complete = False
            print("Mission started - moving to first marker (green)")

    def send_gpio_command(self, command):
        try:
            self.client.publish(self.robot_topic, command)
            print(f"Command sent: {command}")
        except Exception as e:
            print(f"Command sending error: {e}")


    def process_frame(self):
        ret, frame = self.cap.read()
        if not ret:
            print("Failed to capture frame")
            return False

        frame = cv2.flip(frame, 1)
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        red_mask = cv2.inRange(hsv, *self.color_ranges['red'])
        blue_mask = cv2.inRange(hsv, *self.color_ranges['blue'])
        self.front = self.find_center(red_mask)
        self.back = self.find_center(blue_mask)

        if self.mission_active and self.vision_enabled:
            target_color = self.current_sequence[self.current_marker_index]
            target_mask = cv2.inRange(hsv, *self.color_ranges[target_color])
            self.target = self.find_center(target_mask)
        else:
            self.target = (-1, -1)

        status = "AUTO" if self.vision_enabled else "MANUAL"
        cv2.putText(frame, f"Mode: {status}", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

        if self.mission_active:
            target_status = f"Target: {self.current_sequence[self.current_marker_index]}"
            cv2.putText(frame, target_status, (10, 60),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        self.draw_detections(frame)

        if self.show_camera:
            cv2.imshow(self.window_name, frame)

        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            self.shutdown()
        elif key == ord('v'):
            self.vision_enabled = not self.vision_enabled
            print(f"Vision mode {'enabled' if self.vision_enabled else 'disabled'} by keyboard")
        elif key == ord('m'):
            self.start_mission()

        return True

    def draw_detections(self, frame):
        if self.front[0] >= 0 and self.back[0] >= 0:
            cv2.circle(frame, self.front, 10, (0, 0, 255), -1)
            cv2.circle(frame, self.back, 10, (255, 0, 0), -1)
            cv2.line(frame, self.back, self.front, (255, 255, 255), 2)
            cv2.putText(frame, "Front", (self.front[0] + 15, self.front[1]),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)
            cv2.putText(frame, "Back", (self.back[0] + 15, self.back[1]),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 1)

        if self.target[0] >= 0 and self.mission_active:
            color_map = {
                'green': (0, 255, 0),
                'yellow': (0, 255, 255),
                'white': (255, 255, 255)
            }
            target_color = self.current_sequence[self.current_marker_index]
            color = color_map.get(target_color, (0, 255, 0))
            cv2.circle(frame, self.target, 15, color, 2)
            cv2.putText(frame, target_color, (self.target[0] + 20, self.target[1]),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)
            if self.front[0] >= 0:
                cv2.line(frame, self.front, self.target, color, 1)

    def find_center(self, mask):
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        if not contours:
            return (-1, -1)

        largest = max(contours, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(largest)
        return (x + w // 2, y + h // 2)

    def get_angle_to_target(self):
        if self.front[0] < 0 or self.back[0] < 0 or self.target[0] < 0:
            return 360.0

        robot_dir = (self.front[0] - self.back[0], self.front[1] - self.back[1])
        to_target = (self.target[0] - self.front[0], self.target[1] - self.front[1])

        cross = robot_dir[0] * to_target[1] - robot_dir[1] * to_target[0]
        dot = robot_dir[0] * to_target[0] + robot_dir[1] * to_target[1]

        return np.arctan2(cross, dot) * 180 / np.pi

    def get_distance_to_target(self):
        if self.front[0] < 0 or self.target[0] < 0:
            return -1.0
        return np.sqrt((self.target[0] - self.front[0]) ** 2 + (self.target[1] - self.front[1]) ** 2)


    def vision_control_loop(self):
        angle_threshold = 7.0
        distance_threshold = 60.0

        while self.running and self.vision_enabled:
            if not self.process_frame():
                time.sleep(0.1)
                continue

            if not self.mission_active:
                time.sleep(0.1)
                continue

            if self.target[0] < 0:
                print("Searching for target...")
                self.send_gpio_command("stop")
                time.sleep(0.1)
                continue

            angle = self.get_angle_to_target()
            distance = self.get_distance_to_target()
            print(f"Target info - Angle: {angle:.1f}°, Distance: {distance:.1f} px")

            if distance <= distance_threshold:
                print(f">>> Marker {self.current_sequence[self.current_marker_index]} reached! <<<")

                if self.current_sequence[self.current_marker_index] == "white":
                    print("At conveyor - starting loading")
                    self.send_gpio_command("stop")
                    self.client.publish(self.conveyor_topic, "start_loading")

                    while not self.loading_complete and self.running:
                        time.sleep(0.1)

                    if self.loading_complete:
                        print("Loading complete - returning home")
                        self.current_sequence = self.backward_sequence
                        self.current_marker_index = 0
                        self.loading_complete = False
                elif self.current_sequence[self.current_marker_index] == "green" and self.current_sequence == self.backward_sequence:
                    print("Mission complete - robot home")
                    self.mission_active = False
                    self.send_gpio_command("stop")
                else:
                    self.current_marker_index += 1
                    print(f"New target: {self.current_sequence[self.current_marker_index]}")

            elif abs(angle) > angle_threshold:
                if angle > 0:
                    self.send_gpio_command("left")
                    time.sleep(0.1)
                else:
                    self.send_gpio_command("right")
                    time.sleep(0.1)
            else:
                self.send_gpio_command("forward")
                time.sleep(0.3)

    def start(self):
        try:
            self.client.connect(self.broker_ip, self.port, 60)
            self.running = True

            signal.signal(signal.SIGINT, self.shutdown)
            signal.signal(signal.SIGTERM, self.shutdown)

            mqtt_thread = threading.Thread(target=self.client.loop_forever)
            mqtt_thread.daemon = True
            mqtt_thread.start()

            print(f"System started. Waiting for commands on '{self.laptop_topic}'...")
            print("Commands available:")
            print(" - start_mission: Begin marker following mission")
            print(" - start_vision: Enable autonomous mode")
            print(" - stop_vision: Disable autonomous mode")
            print(" - toggle_camera: Show/hide camera window")
            print(" - shutdown: Stop the system")
            print("Keyboard shortcuts:")
            print(" - 'q': Quit")
            print(" - 'v': Toggle vision mode")
            print(" - 'm': Start mission")

            while self.running:
                self.process_frame()
                if self.vision_enabled:
                    self.vision_control_loop()
                else:
                    time.sleep(0.05)

        except Exception as e:
            print(f"Startup error: {e}")
        finally:
            self.cleanup()

    def shutdown(self, signum=None, frame=None):
        print("\nShutting down system...")
        self.send_gpio_command("stop")
        self.running = False
        self.vision_enabled = False


    def cleanup(self):
        self.client.disconnect()
        self.cap.release()
        cv2.destroyAllWindows()
        print("System stopped")


if __name__ == "__main__":
    try:
        system = RobotControlSystem()
        system.start()
    except Exception as e:
        print(f"Fatal error: {e}")
        sys.exit(1)
