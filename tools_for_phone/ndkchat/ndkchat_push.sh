. setenv-android-tools.sh 
cd ~/Vendors/andorid/MSM7627_froyo_almond_60501005/
#adb push out/target/product/msm7627_surf/system/bin/servicemanager /system/bin
#adb push out/target/product/msm7627_surf/system/bin/service /data
adb push out/target/product/msm7627_surf/system/bin/comchatClient /data
adb push out/target/product/msm7627_surf/system/bin/comchatServer /data
adb push out/target/product/msm7627_surf/system/bin/comchatSvrLoader /data
adb push out/target/product/msm7627_surf/system/lib/libcomchatPS.so /data
adb push out/target/product/msm7627_surf/system/lib/libcomchatService.so /data
adb shell chmod 777 /data/comchat*
