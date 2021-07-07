import multiprocessing as mp
import time
import argparse
import os, tempfile
import numpy as np
import xml.etree.ElementTree as ET
import cv2
import cv2.aruco as aruco
from robomaster import robot
from robomaster import camera


# The motion thread connects with Robomaster and controls the motion
# of the robot.
#
def motion_thread(ref_x, ref_y, action_queue: mp.Queue,
                  args):
    pass


# The communication thread establishes connection between the
#   Robomaster and the root node (on localhost), and achieves
def comm_thread(cam_x, cam_y, cam_z, action_queue: mp.Queue,
                args):
    pass


# The vision thread connects with Robomaster and opens video stream
# for detecting reference ArUco markers. After the ref marker has been
# detected, the vision_thread
#   puts (Camera Coordinates of aruco marker, Estimated Self Position)
#   into the pose_queue (multiprocessing.Queue) for the use of comm and
#   motion routines
# To estimate the location based on ArUco marker, the camera
# calibration parameters and the reference location of reference
# ArUco marker must be provided when the program starts
def vision_thread(cam_x, cam_y, cam_z, ref_x, ref_y,
                  args):
    ep_robot = robot.Robot()
    ep_robot.initialize(conn_type="ap")

    # Constant parameters used in Aruco methods
    ARUCO_PARAMETERS = aruco.DetectorParameters_create()
    ARUCO_DICT = aruco.Dictionary_get(aruco.DICT_ARUCO_ORIGINAL)

    # Load camera intrinsic parameters
    fs = cv2.FileStorage(args.calibration_config, cv2.FILE_STORAGE_READ)
    camera_matrix = fs.getNode("camera_matrix").mat()
    distortion = fs.getNode("distortion_coefficients").mat()

    # Initialize ep camera
    ep_camera = ep_robot.camera
    ep_camera.start_video_stream(display=False)
    cam_x, cam_y, cam_z = 0.0, 0.0, 0.0
    while True:
        # Capturing each frame of our video stream
        QueryImg = ep_camera.read_cv2_image()
        # grayscale image
        gray = cv2.cvtColor(QueryImg, cv2.COLOR_BGR2GRAY)

        # Detect Aruco markers
        corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, ARUCO_DICT, parameters=ARUCO_PARAMETERS)
        refmark_id = -1
        if ids is not None:
            for i in range(len(ids)):
                if ids[i] == args.ref_id:
                    refmark_id = i

        if refmark_id != -1:
            rvecs, tvecs, objPoints = aruco.estimatePoseSingleMarkers(corners, 0.5, camera_matrix, distortion)
            rvec, tvec = rvecs[refmark_id], tvecs[refmark_id]
            rotM = cv2.Rodrigues(rvecs[refmark_id])[0]
            cam_x, cam_y, cam_z = np.dot(-np.matrix(rotM).T, tvec.T)

        # for debug use
        cv2.putText(QueryImg, 'cam_x: {}'.format(cam_x), (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 1,
                    cv2.LINE_AA)
        cv2.putText(QueryImg, 'cam_y: {}'.format(cam_y), (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 1,
                    cv2.LINE_AA)
        cv2.putText(QueryImg, 'cam_z: {}'.format(cam_z), (50, 150), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 1,
                    cv2.LINE_AA)
        cv2.imshow('QueryImage', QueryImg)
        if cv2.waitKey(1) & 0xff == ord('q'):
            break


if __name__ == '__main__':
    # parse command line arguments
    parser = argparse.ArgumentParser(description='Robomaster Daemon: Data processing and control on Robomaster')
    parser.add_argument('-c', '--calibration_config', metavar='out.xml', type=str, required=True,
                        help="Camera calibration file")
    parser.add_argument('-f', '--gps_ref', metavar='gpsref.xml', type=str, required=True,
                        help="GPS reference file")
    parser.add_argument('-r', '--ref_id', metavar='ArUco ID', type=int, required=True,
                        help="ArUco marker used for reference")
    args = parser.parse_args()

    # define shared variables
    action_queue = mp.Queue(100)
    cam_x, cam_y, cam_z, ref_x, ref_y = [mp.Value('d', 0) for i in range(5)]

    # define detached processes
    vision_process = mp.Process(target=vision_thread, args=(cam_x, cam_y, cam_z, ref_x, ref_y, args,))
    motion_process = mp.Process(target=motion_thread, args=(ref_x, ref_y, action_queue, args,))
    comm_process = mp.Process(target=comm_thread, args=(cam_x, cam_y, cam_z, action_queue, args,))

    # launch processes
    vision_process.start()
    motion_process.start()
    comm_process.start()
    vision_process.join()
    motion_process.join()
    comm_process.join()
