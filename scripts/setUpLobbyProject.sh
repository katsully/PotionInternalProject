#!/bin/bash

sudo cp com.po.LobbyProjectApp.plist ~/Library/LaunchAgents
launchctl load -w ~/Library/LaunchAgents/com.po.LobbyProjectApp.plist
