# PTZFaceTracker

This application can be used to automatically track a face using an IP camera with Pan & Tilt functionality. It detects a face using OpenCV and tracks the face with the tracking module of OpenCV. Whenever the tracked face approaches the borders of the video, the application tells the camera to pan or tilt so that the face will be back in the center of the video.
The application has been developed and tested to work with the [Vstarcam C7837WIP HD 720P](http://www.vstarcam.com/C7837WIP-Home-monitoring-IP-Camera-121.html), but it should also work for other IP cameras.

## Compilation

This code requires [OpenCV](https://github.com/opencv/opencv) and [OpenCV's extra modules](https://github.com/opencv/opencv_contrib). On Linux, OpenCV and the tracking module can be made available with

```
cd <path to opencv-3.2.0> 
cmake -D OPENCV_EXTRA_MODULES_PATH=<path to opencv_contrib-3.2.0>/modules/ .
make
sudo make install
```

The ```PTZFaceTracker``` can be compiled with


```
cmake .
make
```

## Configuration

The application needs to be configured to be able to communicate with the IP camera. This is done using a configuration file. The file ```example.cfg``` provides a sample configuration.

```
IP	10.10.10.10
PORT	81
PTZ_LEFT	/decoder_control.cgi?loginuse=<username>&loginpas=<password>&command=4&onestep=0
PTZ_RIGHT	/decoder_control.cgi?loginuse=<username>&loginpas=<password>&command=6&onestep=0
PTZ_UP		/decoder_control.cgi?loginuse=<username>&loginpas=<password>&command=0&onestep=0
PTZ_DOWN 	/decoder_control.cgi?loginuse=<username>&loginpas=<password>&command=2&onestep=0
PTZ_STOP 	/decoder_control.cgi?loginuse=<username>&loginpas=<password>&command=1&onestep=0
```

The parameters ```IP``` and ```PORT``` give the IP address and port that can be used to control the camera via http. The ```PTZ_*``` parameters should provide the commands to pan and tilt the camera.
These commands can be identified by analysing the web interface of the camera. For a similar camera, this is described in detail in the article [Hack a $30 WiFi Pan-Tilt Camera](http://www.instructables.com/id/Hack-a-30-WiFi-Pan-Tilt-Camera-Video-Audio-and-Mot/). 

Furthermore, the applications needs the video stream of the camera to be redirected to a standard video device. This can be achieved using [v4l2loopback](https://github.com/umlaeute/v4l2loopback) and Gstreamer. 

```
sudo modprobe v4l2loopback
gst-launch -vet  rtspsrc user-id='<udsername>' user-pw='<password>' location='rtsp://10.10.10.10:10554/udp/av0_0' latency=0 ! decodebin2 ! v4l2sink device=/dev/video1
```

Of course, the IP address and the port used for ```rtsp``` may have to be changed. You should be able to obtain the correct ```rtsp``` location from the web interface of your camera.

## Usage

```
./PTZFaceTracker <video device> <config file>
```

The application automatically tries to detect a face and track it. You can use the left, right, up, and down keys to manually pan and tilt the camera. The space bar can be used to stop pan and tilt and to select a different face, if multiple faces are detected. With the enter key you can make the application track the currently selected face.

## License

MIT


