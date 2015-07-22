# Potion Internal Project

By Luobin Wang and Kat Sullivan


This project was developed to create a more interactive and inviting lobby space at the Potion office. It combines motion tracking with mesh manipulation.

## Getting Started

###CINDER

Cinder must be cloned in the same directory as the Potion Internal Project, and you need to be on the release branch (as of 07/06/15). 

###CINDER BLOCKS

Clone Cinder-OpenNI (https://github.com/wieden-kennedy/Cinder-OpenNI) and Cinder-OpenCV (https://github.com/cinder/Cinder-OpenCV) into Cinder/blocks. Then rename Cinder-OpenCV to just OpenCV.

###HOW TO SETUP A MAC TO USE THE CAMERA

1. Upgrade Xcode by opening the application and selecting the menu option *Xcode/Open Developer Tool/More Developer Tools..*. Clicking this will take you to the Apple Developer website. Log in and continue until you see the **Downloads for Apple Developers** screen. In the list of downloads, search for the most recent version of *Command Line Tools for Xcode*. Click on the blue disk image link to the right of the description to download.
![picture](../../images/install-mavericks-command-line-tools.png?raw=true)
Once you've downloaded the Command Line Tools, simply open the disk image and install the package. Now you're up to date!

2. Make sure you have homebrew installed. Then type the following:

~~~powershell
brew install libtool
~~~
and then

~~~powershell
brew install libusb --universal
~~~

3. Now we have to install all the SDKs. I recommend first creating a Kinect folder in your home directory.
4. First install OpenNI here https://mega.co.nz/#!Hc5kwAiZ!uJiLY4180QGXjKp7sze8S3eDVU71NHiMrXRq0TA7QpU . Double click to uncompress and make sure the directory is in your new Kinect folder. In the terminal navigate to the OpenNI directory, once you are in the directory type

```
sudo ./install.sh
```
if this worked you should see *** DONE *** as the last line.

5. Next we have to install SensorKinect. First, to prevent errors during installation, open the terminal and enter this

```
sudo ln -s /usr/local/bin/niReg /usr/bin/niReg
```

Then go to the SensorKinect github page [here](https://github.com/avin2/SensorKinect) and click the *Download Zip* button. Once downloaded, uncompress *SensroKinect-unstable.zip* and move it to your Kinect folder. Navigate to *SensorKinect093-Bin-MacOSX-v5.1.2.1.tar.bz2* file inside the *Bin* folder, and uncompress it. In the terminal navigate to *Sensor-Bin-MacOSX-v5.1.2.1*. Finally, enter

```
sudo ./install.sh
```

If everything worked you should see *** DONE *** at the bottom of the terminal.

6. The last thing you need to install in NiTE. Download it [here](https://mega.co.nz/#!nZYwgJiQ!m091FXc4U6GwjRfpHK-puPvBjkHdWc6KmQH-_RzXfOw). Just like before, uncompress it and move it to your Kinect folder. Navigate to the *NITE-Bin-Dev-Mac)SX-v1.5.1.21* folder in the terminal, and enter 

```
sudo ./install.sh
```
And look for the *** DONE *** at the end of the installation to know everything worked. 

Now you're ready to go!
 
## Getting the Application Running with Cinder-OpenNI

1. Open the project in XCode. Add the Cinder-OpenNI's lib files (not the lib folder itself) to the Resources Folder in XCode. (NiTE2 and OpenNI2 need to be folder references, not group folders - you are prompted to choose between the two when you drag the files into Resources). These files need to be copied in XCode and in Finder.
2. Copy Cinder-OpenNI's src folder into Blocks (this can be a group folder).
3. Select the Project and then select Build Phases. Link Binary with Libraries should only have libNiTE2.dylib and libOpenNI.dylib in addition to the frameworks already there (for a total of 10 items).
4. The Copy Files section needs libNiTE2.dylib, libOpenNI2.dylib, NiTE.ini, OpenNI.ini, OpenNI2, and NiTE2 with the destination set to Executables. (For a total of 6 items). Make sure Code Sign on Copy is unchecked.
5. Copy Bundle Resources should only have CinderApp.icns.

![photo of xcode setup](../../images/xcode.png?raw=true)
 