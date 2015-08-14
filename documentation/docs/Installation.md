### Installation

To keep this running as a pernament installation, the following machine configurations were made:

1. Set the desktop background to something neutral. For our installation we chose a still from the assets folder.
2. Make sure the computer, the display, and the hard disks never sleep. The computer should also start automatically when the computer freezes.
3. Add a schedule to Energy Save so that the computer starts up and shuts down at a fixed time everyday.
4. Make sure the screen saver never starts.
5. Hide all icons on your desktop. To do this, type this into the terminal
```
defaults write com.apple.finder CreateDesktop -bool false
```
To have all the icons appear again, type this
```
defaults write com.apple.finder CreateDesktop -bool true
```
6. Hide the dock by pressing Option + Command + D while in Finder.
7. Select Automatic Login when your computer starts.
8. Add your application to the Login Items of the user you log in with, to ensure your installation will start up automatically.
9. Disable Bluetooth, Universal Access, and Alert sounds. Also mute the output volume.
10. Disable dashboard, as this takes up resources (CPU). Type this into the terminal
```
defaults write com.apple.dashboard mcx-disabled -boolean YES
```
To enable it again, type this
```
defaults write com.apple.dashboard mcx-disabled -boolean NO
```
11. Disable Spotlight. Type this into your terminal
```
sudo mdutil -a -i on
```
To enable it again, type this
```
sudo mdutil -a -i on
```
12. Disable the Require Password After Sleep in the General tab under Security & Privacy to prevent the system from asking password after a scheduled system start up.


We also prevented OS X from re-opening apps after a restart. We wanted to do this because it increases the time it takes to start up the machine into a steady state, and it re-opens apps you may not be using anymore.

To prevent apps from re-opening, do the following:

1. Quit all apps
2. Navigate to ~/Library/Preferences/ByHost/com.apple.loginwindow.*.plist.
3. Click Command + I on the file, and lock it using the Locked checkbox.

#### Scripts

**There is a plist file called com.po.LobbyProjectApp.plist in the scripts folder. This needs to be moved to ~/Library/LaunchAgents.** This file will ensure that the app will launch at startup and relaunch on a crash.

You will also find two bash scripts in the scripts directory: setUpLobbyProject.sh and tempUnloadLaunchAgent.sh. setUpLobbyProject.sh will copy the launch agents from ~/Library/LaunchAgents. tempUnloadLaunchAgent will undo this action until the computer is restarted. This can come in handy when debugging.