### Research

#### Cinder-Kinect Compatibilty

1. **Cinder-Kinect (https://github.com/cinder/Cinder-Kinect)**: This compiled but it did not regnozied the Kinect. However, this did work with the Asus xtion camera.
2. **Kinect-KCB (https://github.com/wieden-kennedy/Cinder-KCB)**: Windows only
3. **libfreenect (https://github.com/OpenKinect/libfreenect)**: All examples were in C, however I could get some of the executables in the build/bin directory to run. (This only worked with the Kinect, not with the Asus xtion camera).
4. **libfreenect2 (https://github.com/OpenKinect/libfreenect2)**: The protonect example works with Kinect v2.
5. **Heister's Cinder-OpenNI (https://github.com/heisters/Cinder-OpenNI)**: Only works with the xtion camera, not with the Kinect. It also seems to lag every ten seconds. 
6. **Wieden-Kennedy's Cinder-OpenNI (https://github.com/wieden-kennedy/Cinder-OpenNI)**: This also only works with the xtion camera, not the Kinect. For the BasicApp sample, replace 

```
mDevice = mDeviceManager->createDevice();
```

with 

```
mDevice = mDeviceManager->createDevice( OpenNI::DeviceOptions().enableColor() );
```

see https://github.com/wieden-kenedy/Cinder-OpenNI/issues/1. For the UserApp sample, copy the NiTE2 folder (the folder, not just the files) from lib and paste them to samples/UserApp/xcode/build/Debug.

#### Cinder-OpenCV
You may need to add libz.dylib to 'Link Binary with Libraries' in Build Phases.

#### JsonCpp
This is a C++ library for interacting with JSON. We used this to set up the gui parameters as a json file. Checkout the github repo here: (https://github.com/open-source-parsers/jsoncpp). Clone the repo into Project/blocks. From the terminal, cd into the json cpp directory. Then run this command: 

```
python amalgamate.py
```

This generates a single header and source file that make it easier to include into your project. Once you run the script, you add 

```
#include json/json.h 
```

to your project. You may also need to add ../blocks/jsoncpp/dist to user header search paths in your build settings.




