### Toggle Buttons

'f' - toggle fullscreen on and off and having the LobbyProject App being on top of all other apps on and off

'p' - toggle params on and off

'o' - toggle the motion tracking points on and off

's' - toggle between drawing motion tracking points and polygons (polygon is drawn by connecting the points). 'o' will need to be toggled on for this to have any effect.

'c' - toggle showing subview of depth camera

### Data
The assets directory (LobbyProject/assets) is where the app retrieves all photos and videos that get displayed. Any photo or video added to the assets directory will be added to the rotating display of images. All videos must by mp4. 

Also in the assets directory there is a file called gui_params.json. This contains key value pairs for all initial value of the parameters shown in the params window. Here are the parameters in the file:

mSceneRot - edit the x,y,and z-axis of the scene's rotation

volumeMin - the near plane used to set the perspective for Cinder's camera

drawMesh - whether or not the mesh is drawn, by not drawing the mesh it allows you to see the points better (toggle 'o' to see points)

timerInterval - how long for the app to transition photos

mNearLimit - the lower threshold for depth value (in millimeters)

mFarLimit - the upper threshold for depth value (in millimeters)

mThresh - the lower threshold for the camera

mMaxVal - the upper threshold for the camera (the max this can be is 255)

meshResolution - 

