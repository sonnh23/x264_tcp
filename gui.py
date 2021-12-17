import os

from PyQt5 import QtCore
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QHBoxLayout, QVBoxLayout, QLabel, \
    QSlider, QStyle, QSizePolicy, QFileDialog, QLineEdit
import sys, time
from PyQt5.QtMultimedia import QMediaPlayer, QMediaContent
from PyQt5.QtMultimediaWidgets import QVideoWidget
from PyQt5.QtGui import QIcon, QPalette
from PyQt5.QtCore import Qt, QUrl
from PyQt5.QtCore import QThread, QObject, pyqtSignal

class ThreadClass(QtCore.QThread):
    signal = pyqtSignal(int)

    def __init__(self, index=0):
        super().__init__()
        self.index = index

    def run(self):
        print('Starting thread...', self.index)
        counter = 0
        while True:
            time.sleep(1)
            file1 = open("ctalk.txt", "r+")
            data = file1.read()
            print ("Length file is: ",len(data))
            if (len(data)==0):
                counter = -2
            elif (len(data)==1 or len(data)==2):
                if data == "-1":
                    counter = -1 #File is not existed
                elif data == "0":
                    counter = 0 #Error in transferring
                elif data == "1":
                    counter = 1 #OK

            file1.truncate(0)
            file1.close()
            self.signal.emit(counter)

class Window(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("PyQt5 Media Player")
        self.setGeometry(350, 100, 700, 500)
        self.setWindowIcon(QIcon('player.png'))

        p =self.palette()
        p.setColor(QPalette.Window, Qt.white)
        self.setPalette(p)

        self.init_ui()

        self.show()

    def init_ui(self):
        #create media player object
        self.mediaPlayer = QMediaPlayer(None, QMediaPlayer.VideoSurface)
        self.video_valid = False
        #create videowidget object
        videowidget = QVideoWidget()
        videowidget.setStyleSheet('background-color: black')
        self.thread = {}
        #create IP and Port label
        self.IpBar = QLineEdit()
        self.IpBar.setObjectName("ip")
        self.PortBar = QLineEdit()
        self.PortBar.setObjectName("port")
        #create open button
        openBtn = QPushButton('Open Video')
        openBtn.clicked.connect(self.open_file)

        #create search button
        self.searchButton = QPushButton('Search')
        self.searchButton.clicked.connect(self.start_worker_1)

        #create connect and disconnect button
        self.connectButton = QPushButton('Connect')
        #self.searchButton.clicked.connect(self.searchVideo)
        self.disconnectButton = QPushButton('Disconnect')
        #self.disconnectButton.clicked.connect(self.searchVideo)

        #create button for playing
        self.playBtn = QPushButton()
        self.playBtn.setEnabled(False)
        self.playBtn.setIcon(self.style().standardIcon(QStyle.SP_MediaPlay))
        self.playBtn.clicked.connect(self.play_video)

        #create slider
        self.slider = QSlider(Qt.Horizontal)
        self.slider.setRange(0,0)
        self.slider.sliderMoved.connect(self.set_position)

        #create search bar
        self.searchBar = QLineEdit()
        self.searchBar.setObjectName("search")

        #create label
        self.label = QLabel()
        self.label.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Maximum)
        self.label.setStyleSheet('color:black')

        #create hbox layout
        hboxLayout = QHBoxLayout()
        hboxLayout.setContentsMargins(0,0,0,0)

        # create hbox layout
        hboxLayout2 = QHBoxLayout()
        hboxLayout2.setContentsMargins(0, 0, 0, 0)
        hboxLayout2.addWidget(self.searchBar)
        hboxLayout2.addWidget(self.IpBar)
        hboxLayout2.addWidget(self.PortBar)

        # create hbox layout
        hboxLayout3 = QHBoxLayout()
        hboxLayout3.setContentsMargins(0, 0, 0, 0)
        hboxLayout3.addWidget(self.searchButton)
        hboxLayout3.addWidget(self.connectButton)
        hboxLayout3.addWidget(self.disconnectButton)

        #set widgets to the hbox layout
        hboxLayout.addWidget(openBtn)
        hboxLayout.addWidget(self.playBtn)
        hboxLayout.addWidget(self.slider)

        #create vbox layout
        vboxLayout = QVBoxLayout()
        vboxLayout.addLayout(hboxLayout2)
        vboxLayout.addLayout(hboxLayout3)
        vboxLayout.addWidget(videowidget)
        vboxLayout.addLayout(hboxLayout)
        vboxLayout.addWidget(self.label)

        self.setLayout(vboxLayout)
        self.mediaPlayer.setVideoOutput(videowidget)

        #media player signals
        self.mediaPlayer.stateChanged.connect(self.mediastate_changed)
        self.mediaPlayer.positionChanged.connect(self.position_changed)
        self.mediaPlayer.durationChanged.connect(self.duration_changed)

    def start_worker_1(self):
        self.thread[1] = ThreadClass(index=1)
        self.thread[1].start()
        self.thread[1].signal.connect(self.my_function)
        #self.searchButton.setEnabled(False)
        self.searchButton.setEnabled(True)

    def my_function(self, counter):
        m = counter
        file_py = open("pytalk.txt", "w")
        video_name = self.searchBar.text()
        file_py.write(video_name)
        file_py.close()
        if m == -1:
            self.label.setText("File is not existed")
        elif m == 0:
            self.label.setText("Error in the transfer process")
        elif m == -2:
            self.label.setText("C chua noi gi ca")
            #self.label.hide()
        elif m == 1:
            self.label.setText("Playing Video")
            filename, _ = QFileDialog.getOpenFileName(self, "Open Video")
            if filename != '':
                self.mediaPlayer.setMedia(QMediaContent(QUrl.fromLocalFile(filename)))
                self.playBtn.setEnabled(True)

    def open_file(self):
        filename, _ = QFileDialog.getOpenFileName(self, "Open Video")
        if filename != '':
            self.mediaPlayer.setMedia(QMediaContent(QUrl.fromLocalFile(filename)))
            self.playBtn.setEnabled(True)

    def play_video(self):
        if self.mediaPlayer.state() == QMediaPlayer.PlayingState:
            self.mediaPlayer.pause()
        else:
            self.mediaPlayer.play()

    def mediastate_changed(self, state):
        if self.mediaPlayer.state() == QMediaPlayer.PlayingState:
            self.playBtn.setIcon(
                self.style().standardIcon(QStyle.SP_MediaPause)
            )
            self.searchBar.hide()

        else:
            self.playBtn.setIcon(
                self.style().standardIcon(QStyle.SP_MediaPlay)
            )
            self.searchBar.show()

    def position_changed(self, position):
        self.slider.setValue(position)

    def duration_changed(self, duration):
        self.slider.setRange(0, duration)

    def set_position(self, position):
        self.mediaPlayer.setPosition(position)

    def handle_errors(self):
        self.playBtn.setEnabled(False)
        self.label.setText("Error: " + self.mediaPlayer.errorString())

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Window()
    sys.exit(app.exec_())