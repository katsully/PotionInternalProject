# Potion Internal Project

By Luobin Wang and Kat Sullivan


This project was developed to create a more interactive and inviting lobby space at the Potion office. It combines motion tracking with mesh manipulation.

## Getting Started

###CINDER

Cinder must be cloned in the same directory as the Potion Internal Project, and you need to be on the release branch (as of 07/06/15). 

###CINDER BLOCKS

Clone Cinder-OpenNI (https://github.com/wieden-kennedy/Cinder-OpenNI) and Cinder-OpenCV (https://github.com/cinder/Cinder-OpenCV) into Cinder/blocks.  Then rename Cinder-OpenCV to just OpenCV.

###USING THE CAMERA

To get the camera working follow this blog post: http://blog.nelga.com/setup-microsoft-kinect-on-mac-os-x-10-9-mavericks/. However, in Step 2 don't install Macports, use homebrew instead.

So, in Step 3, instead of 

~~~powershell
sudo port install libtool
~~~
do

~~~powershell
brew install libtool
~~~
and instead of 

~~~powershell
sudo port install libusb +universal
~~~
do

~~~powershell
brew install libusb --universal
~~~

## Getting the Application Running with Cinder-OpenNI

1. Open the project in XCode. Add the Cinder-OpenNI's lib files (not the lib folder itself) to the Resources Folder in XCode. (NiTE2 and OpenNI2 need to be folder references, not group folders - you are prompted to choose between the two when you drag the files into Resources). These files need to be copied in XCode and in Finder.
2. Copy Cinder-OpenNI's src folder into Blocks (this can be a group folder).
3. Select the Project and then select Build Phases. Link Binary with Libraries should only have libNiTE2.dylib and libOpenNI.dylib in addition to the frameworks already there (for a total of 10 items).
4. The Copy Files section needs libNiTE2.dylib, libOpenNI2.dylib, NiTE.ini, OpenNI.ini, OpenNI2, and NiTE2 with the destination set to Executables. (For a total of 6 items). Make sure Code Sign on Copy is unchecked.
5. Copy Bundle Resources should only have CinderApp.icns.

![photo of xcode setup](images/xcode.png?raw=true)
 