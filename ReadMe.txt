
1. google
2. maps
3. youtube
4. music
5. video
6. volume
7. take a snap
8. stream a video
9. date
10. Weather
11. WHAMI
12. TIME
13. Open Folder
14. Mail
15. Browser
16. Logoff
17. Poweroff












xdg-open "http://www.google.com/search?q=Vaibhav Dhingani"
xdg-open "https://www.youtube.com/results?search_query=Nirvana Songs"
xdg-open "https://www.google.com/maps?q=Vedanta Platina"
notify-send 'DeViz' 'Starting Music'
streamer -f jpeg -s 1920x1080 -o image.jpeg


vlc -vvv ~/Desktop/source.mp4 --sout="#transcode{vcodec=h264,vb=800,scale=1,acodec=mp4a,ab=128,channels=2,samplerate=44100}:rtp{sdp=rtsp://:8554/live.ts}" -I dummy

vlc -vvv ~/Desktop/source.mp4 --sout '#transcode{vcodec=mp2v,vb=800,acodec=none}:rtp{dst=10.245.227.102,port=5004,mux=ts}'
rtp://10.245.227.102:5004/




gst-launch videotestsrc ! ffenc_mpeg4 ! rtpmp4vpay send-config=true ! udpsink host=10.245.227.102 port=5000
gst-launch -v v4l2src device=/dev/video0 ! video/x-raw-yuv,framerate=30/1,width=1280,height=720 ! ffenc_mpeg4 ! rtpmp4vpay send-config=true ! udpsink host=10.245.227.102 port=5000
.sdp file
=========
v=0
m=video 5000 RTP/AVP 96
c=IN IP4 10.245.227.102
a=rtpmap:96 MP4V-ES/90000



gst-launch videotestsrc ! video/x-raw-yuv,framerate=30/1,width=1280,height=720 ! ffenc_mpeg4 ! rtpmp4vpay send-config=true ! udpsink host=127.0.0.1 port=5000
gst-launch -v v4l2src device=/dev/video0 ! video/x-raw-yuv,framerate=30/1,width=1280,height=720 ! ffenc_mpeg4 ! rtpmp4vpay send-config=true ! udpsink host=127.0.0.1 port=5000


Notify Me
=========
1. Basic message with an icon in front of the Title showing 5000 milliseconds
Code:
$ notify-send "Message Title" "The message body is shown here" -i /usr/share/pixmaps/idle.xpm -t 5000
2. Show contents of a file:
Code:
$ notify-send test "`tail /var/log/syslog`"
3. Follow log file:
Code:
$ tail -n0 -f /var/log/messages | while read line; do notify-send "System Message" "$line"; done
4. Format message with HTML:
Code:
$ notify-send Test "<font size=16 color=blue><b><i>Hello World</b></i></font>"
5. Notification with custom icon
You can use your own custom icon using the notify-send -i option as shown below.
$ notify-send -i /home/sathiya/deal.ico 'Deal success'
$ notify-send -i /home/vaibhavdhingani/home/MyAIapp/AI/image.jpeg "Pic" "Pic1"

6. Date
$ notify-send Date "`date`"
