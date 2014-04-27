#!/bin/bash
echo This is script for deploy qt+opencv app on MAC OS
cp -R /Users/andheroe/Dropbox/build-Viewaide-Desktop_Qt_5_2_0_clang_64bit-Release/Viewaide.app /Users/andheroe/Desktop
cd Desktop
echo Copy Qt libs
/Users/andheroe/Qt/5.2.0/clang_64/bin/macdeployqt Viewaide.app

echo Copy cascades
cp /Users/andheroe/Dropbox/Viewaide/haarcascade_frontalface_alt_tree.xml Viewaide.app/Contents/Resources
cp /Users/andheroe/Dropbox/Viewaide/haarcascade_mcs_lefteye.xml Viewaide.app/Contents/Resources
cp /Users/andheroe/Dropbox/Viewaide/haarcascade_mcs_righteye.xml Viewaide.app/Contents/Resources

echo Set icon
rm Viewaide.app/Contents/Resources/icon.rc
cp /Users/andheroe/Dropbox/Viewaide/Logo.icns Viewaide.app/Contents/Resources

echo Edit .plist
defaults write /Users/andheroe/Desktop/Viewaide.app/Contents/Info.plist CFBundleIconFile -string Logo.icns
defaults write /Users/andheroe/Desktop/Viewaide.app/Contents/Info.plist LSUIElement -bool true
defaults write /Users/andheroe/Desktop/Viewaide.app/Contents/Info.plist NSHighResolutionCapable -bool true
defaults write /Users/andheroe/Desktop/Viewaide.app/Contents/Info.plist CFBundleIdentifier -string com.viewaide.viewaide
defaults write /Users/andheroe/Desktop/Viewaide.app/Contents/Info.plist CFBundleDisplayName -string Viewaide
defaults write /Users/andheroe/Desktop/Viewaide.app/Contents/Info.plist NSHumanReadableCopyright -string "Copyright 2013-2014 Viewaide, Inc. All rights reserved."

echo Create .dmg
source=Viewaide.app
title=Viewaide
size=100000

hdiutil create -srcfolder "${source}" -volname "${title}" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${size}k viewaide-mac.dmg

device=$(hdiutil attach -readwrite -noverify -noautoopen viewaide-mac.dmg | egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 2

echo Stylize .dmg 
