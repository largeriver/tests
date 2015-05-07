. setenv-android-tools.sh 
cd ~/Vendors/andorid/MSM7627_froyo_almond_60501005/
#adb push out/target/product/msm7627_surf/system/bin/servicemanager /system/bin
#adb push out/target/product/msm7627_surf/system/bin/service /data
adb push out/target/product/msm7627_surf/system/bin/nuClient /data
adb push out/target/product/msm7627_surf/system/bin/nuClient2 /data
adb push out/target/product/msm7627_surf/system/bin/nuSvrLoader /data
adb push out/target/product/msm7627_surf/system/lib/libnuMyServiceCOM.so /data
adb push out/target/product/msm7627_surf/system/lib/libnuMyServicePS.so /data
adb shell chmod 777 /data/nu*
