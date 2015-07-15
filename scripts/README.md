##Viewing Gallery Launch Scripts

**com.po.LobbyProjectApp.plist**: Launches and relaunches LobbyProject.app on crash. This launch agent is placed in ~/Library/LaunchAgents/. *LobbyProject.app needs to be at path: /Users/potion/PotionInternalProject/LobbyProject/xcode/build/Debug/LobbyProject.app*

**setUpLobbyProject.sh**: Copies the launch agents from PotionInternalProject/scripts to /Library/LaunchAgents and ~/Library/LaunchAgents/ and loads them via launchctl.

**tempUnloadLaunchAgent.sh**: Temporarily stops launchagent until the next restart. To be used when debugging/rebuilding app.
