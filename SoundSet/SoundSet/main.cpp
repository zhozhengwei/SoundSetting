#include <windows.h>
#include <iostream>

// ���������ô���
void OpenSoundSettings() {
	// ���������ô���
	WinExec("control mmsys.cpl,,1", SW_NORMAL);
	Sleep(2000); // �ȴ����ڴ�
}

// ��ȡ���ڿͻ����ľ�������
POINT GetAbsoluteCoordinates(HWND hwnd, int x, int y) {
	POINT pt = { x, y };
	ClientToScreen(hwnd, &pt);
	return pt;
}

// �Ҽ����ָ������
void RightClick(HWND hwnd, int x, int y) {
	POINT pt = GetAbsoluteCoordinates(hwnd, x, y);
	// �ƶ���굽ָ�����겢�Ҽ����
	SetCursorPos(pt.x, pt.y);
	mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
	Sleep(500); // �ȴ��������
}

// ����ָ���ļ�
void PressKey(WORD key) {
	// ģ�ⰴ������
	keybd_event(key, 0, 0, 0);
	Sleep(100); // �ȴ��������
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
	Sleep(100); // �ȴ��������
}

int main() {
	OpenSoundSettings();
	// ��ȡ�������ô��ھ��
	HWND hwndSoundSettings = FindWindow(NULL, "Sound"); // �޸�Ϊ�������ô��ڵı���
	if (!hwndSoundSettings) {
		std::cerr << "Failed to find sound settings window." << std::endl;
		return 1;
	}

	// �ȴ��������ô�����ȫ����
	Sleep(300);

	// �Ҽ������һ���豸����ʾ�����õ��豸
	RightClick(hwndSoundSettings, 300, 150); // ����Ϊ����ڴ��ڵ�����
	PressKey(VK_DOWN); // �����ƶ�������ʾ�����õ��豸��
	PressKey(VK_RETURN); // ȷ��ѡ��

	// �Ҽ������˷������豸������
	RightClick(hwndSoundSettings, 300, 150); // ����Ϊ����ڴ��ڵ�����
	PressKey(VK_DOWN); // �����ƶ��������á�ѡ��
	PressKey(VK_RETURN); // ȷ��ѡ��

	return 0;
}
