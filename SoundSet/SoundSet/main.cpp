#include <windows.h>
#include <iostream>

// 打开声音设置窗口
void OpenSoundSettings() {
	// 打开声音设置窗口
	WinExec("control mmsys.cpl,,1", SW_NORMAL);
	Sleep(2000); // 等待窗口打开
}

// 获取窗口客户区的绝对坐标
POINT GetAbsoluteCoordinates(HWND hwnd, int x, int y) {
	POINT pt = { x, y };
	ClientToScreen(hwnd, &pt);
	return pt;
}

// 右键点击指定坐标
void RightClick(HWND hwnd, int x, int y) {
	POINT pt = GetAbsoluteCoordinates(hwnd, x, y);
	// 移动鼠标到指定坐标并右键点击
	SetCursorPos(pt.x, pt.y);
	mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
	Sleep(500); // 等待操作完成
}

// 按下指定的键
void PressKey(WORD key) {
	// 模拟按键操作
	keybd_event(key, 0, 0, 0);
	Sleep(100); // 等待操作完成
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100); // 等待操作完成
}

int main() {
	OpenSoundSettings();
	// 获取声音设置窗口句柄
	HWND hwndSoundSettings = FindWindow(NULL, "Sound"); // 修改为声音设置窗口的标题
	if (!hwndSoundSettings) {
		std::cerr << "Failed to find sound settings window." << std::endl;
		return 1;
	}

	// 等待声音设置窗口完全加载
	Sleep(300);

	// 右键点击第一个设备并显示被禁用的设备
	RightClick(hwndSoundSettings, 300, 150); // 调整为相对于窗口的坐标
	PressKey(VK_DOWN); // 向下移动到“显示被禁用的设备”
	PressKey(VK_RETURN); // 确认选择

	// 右键点击麦克风阵列设备并启用
	RightClick(hwndSoundSettings, 300, 150); // 调整为相对于窗口的坐标
	PressKey(VK_DOWN); // 向下移动到“启用”选项
	PressKey(VK_RETURN); // 确认选择

	return 0;
}
