from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(
    IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
volume = cast(interface, POINTER(IAudioEndpointVolume))
 
 
#判断是否静音，mute为1代表是静音，为0代表不是静音
mute = volume.GetMute()
if mute == 1:
    print('当前是静音状态')
else:
    print('当前是非静音状态')
 
#获取音量值，0.0代表最大，-65.25代表最小
vl = volume.GetMasterVolumeLevel()
print('当前音量值为%s' % vl)
 
#获取音量范围，我的电脑经测试是(-65.25, 0.0, 0.03125)，第一个应该代表最小值，第二个代表最大值，第三个不知道是干嘛的。也就是音量从大到小是0.0到-65.25这个范围
vr = volume.GetVolumeRange()
print('当前音量值为%s' % str(vr))
 
#设置音量, 比如-13.6代表音量是40，0.0代表音量是100
volume.SetMasterVolumeLevel(-13.6, None)
print('已设置音量为40%')