import cv2
from robomaster import robot
from robomaster import camera

ep_robot = robot.Robot()
ep_robot.initialize(conn_type="ap")
ep_camera = ep_robot.camera
ep_camera.start_video_stream(display=False)

img = ep_camera.read_cv2_image()
frame_height, frame_width = img.shape[0], img.shape[1]
out = cv2.VideoWriter('record.avi', cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'),
                      10, (frame_width, frame_height))

while True:
    frame = ep_camera.read_cv2_image()
    out.write(frame)
    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

out.release()
cv2.destroyAllWindows()
