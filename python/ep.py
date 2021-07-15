import math
import robomaster
from robomaster import robot
# import mysql.connector
# robot.config.ROBOT_DEVICE_PORT = 20020
# robot.config.ROBOT_DEFAULT_RNDIS_ADDR = ('10.20.30.188', robot.config.ROBOT_DEVICE_PORT)

robot.config.LOCAL_IP_STR = "192.168.57.104"

class EPVehicle:
    ep_robot = None
    current_xy = [0,0]
    current_z = 0

    def __init__ (self):
        self.ep_robot = robot.Robot() 
       
    def connect(self): 
        self.ep_robot.initialize(conn_type='rndis')
        version = self.ep_robot.get_version()
        print("vehicle connected, version: {0}".format(version))

    def close(self):
        self.ep_robot.close()

    def goto_xy(self, waypoint):
        x_togo = waypoint[0] - self.current_xy[0]
        y_togo = waypoint[1] - self.current_xy[1] 
        print("x_togo", x_togo)
        print("y_togo ", y_togo)
        
        
        z_target = - math.atan2(y_togo, x_togo) / (2*math.pi) * 360
        z_togo = z_target - self.current_z

        z_togo = (z_togo + 180) % 360 - 180

        print("turn: ", z_togo)

        # turn first
        action = self.ep_robot.chassis.move(z=z_togo, z_speed=90)
        action.wait_for_completed()
        self.current_z = self.current_z + z_togo

        # go straight
        xy_togo = math.sqrt(x_togo * x_togo + y_togo * y_togo)
        action = self.ep_robot.chassis.move(x=xy_togo, xy_speed=1)
        action.wait_for_completed()

        self.current_xy[0] = self.current_xy[0] + x_togo
        self.current_xy[1] = self.current_xy[1] + y_togo 

        print("current x: ", self.current_xy[0])
        print("current y: ", self.current_xy[1]) 



    def go_path(self, waypoint_lists): 
        for waypoint in waypoint_lists:
            # go to waypoints
            # action = self.ep_robot.chassis.move(x=waypoint[0], y=waypoint[1], xy_speed=1)
            # action.wait_for_completed()
            self.goto_xy(waypoint)



    def get_current_xy_location(self):
        return self.current_xy
        


# -*- encoding: utf-8 -*-
# 测试环境: Python 3.6 版本

import socket
import sys

# 直连模式下，机器人默认 IP 地址为 192.168.2.1, 控制命令端口号为 40923
# host = "192.168.2.1"
# port = 40923



# from math import atan, pi


if __name__ == '__main__':
    # 如果本地IP 自动获取不正确，手动指定本地IP地址
    # robomaster.config.LOCAL_IP_STR = "192.168.2.20"
    vehicle = EPVehicle()
    vehicle.connect()
    # action = vehicle.ep_robot.chassis.move(y=10, z_speed=30)
    # action.wait_for_completed()
    # vehicle.go_path([[0.4, 0.4],[0.4, -0.4], [-0.8, 0]])
    # for i in range(10):
        # vehicle.go_path([[0.5, 0],[0, 0.5], [0, 0],[0.00001,0]])
    vehicle.close()