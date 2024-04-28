import time
import ctypes
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

# 打开声音设置界面并启用显示被禁用的设备
def open_sound_settings():
    ctypes.windll.shell32.ShellExecuteW(
        None,
        "open",
        "control",
        "mmsys.cpl,,1",
        None,
        1
    )
    time.sleep(1)  # 等待界面打开
    # 发送消息，开启显示被禁用的设备
    ctypes.windll.user32.SendMessageW(
        ctypes.c_long(0xffff), 0x0319, None, ctypes.c_long(2)
    )

# 检查并启用 Recording 选项卡中的 Microphone Array 设备
def enable_microphone_array():
    devices = AudioUtilities.GetSpeakers()  # 获取音频设备列表
    for dev in devices:
        dev_name = dev.GetDisplayName()
        if "Microphone Array" in dev_name:
            endpoint = dev.Activate(
                ctypes.c_ulong(IAudioEndpointVolume._iid_),
                ctypes.c_ulong(0),
                None
            ).QueryInterface(IAudioEndpointVolume)
            endpoint.SetMute(0, None)  # 解除静音，即启用设备
            print("Microphone Array 设备已启用")
            return
    print("Microphone Array 设备不存在或被禁用。")


def main():
    open_sound_settings()  # 打开声音设置界面
    enable_microphone_array()  # 检查并启用 Microphone Array 设备

if __name__ == "__main__":
    main()
