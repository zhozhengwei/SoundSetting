#include <Windows.h>
#include <iostream>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <FunctionDiscoveryKeys_devpkey.h>
#include <Mmdeviceapi.h>
#include <Propidl.h>
#include <propkey.h>
#include <devpkey.h>
#include <Propsys.h>

//devpkey.h Propsys.h
using namespace std;

// 打开声音设置的 Recording 选项卡
void OpenRecordingTab()
{
	// 使用 WinExec 打开控制面板中的声音设置
	WinExec("control mmsys.cpl,,1", SW_NORMAL);
}


// 检查是否存在并启用 Microphone Array 设备
bool CheckMicrophoneArray()
{
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice = NULL;
	IPropertyStore *pPropertyStore = NULL;

	// 初始化COM库
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		cerr << "Failed to initialize COM library" << endl;
		return false;
	}

	// 创建设备枚举器
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
	if (FAILED(hr))
	{
		cerr << "Failed to create device enumerator" << endl;
		CoUninitialize();
		return false;
	}

	// 获取音频输入设备集合
	hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
	if (FAILED(hr))
	{
		cerr << "Failed to enumerate audio endpoints" << endl;
		pEnumerator->Release();
		CoUninitialize();
		return false;
	}

	// 遍历音频输入设备
	UINT count;
	hr = pCollection->GetCount(&count);
	if (FAILED(hr))
	{
		cerr << "Failed to get device count" << endl;
		pCollection->Release();
		pEnumerator->Release();
		CoUninitialize();
		return false;
	}

	bool microphoneArrayExists = false;
	for (UINT i = 0; i < count; ++i)
	{
		hr = pCollection->Item(i, &pDevice);
		if (FAILED(hr))
		{
			cerr << "Failed to get device" << endl;
			continue;
		}

		LPWSTR pwszID = NULL;
		hr = pDevice->GetId(&pwszID);
		if (FAILED(hr))
		{
			cerr << "Failed to get device ID" << endl;
			pDevice->Release();
			continue;
		}

		if (pwszID != NULL)
		{
			PROPERTYKEY propkey = PKEY_Device_FriendlyName;
			PROPVARIANT propvar;
			PropVariantInit(&propvar);

			hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
			if (SUCCEEDED(hr))
			{
				hr = pPropertyStore->GetValue(propkey, &propvar);
				if (SUCCEEDED(hr))
				{
					wstring friendlyName(propvar.pwszVal);
					if (friendlyName.find(L"Microphone Array") != wstring::npos)
					{
						microphoneArrayExists = true;
						printf("Microphone Array found: %ls\n", friendlyName.c_str());
					}
				}
			}

			CoTaskMemFree(pwszID);
			PropVariantClear(&propvar);
			pDevice->Release();
		}
	}

	if (pPropertyStore != NULL)
	{
		pPropertyStore->Release();
	}

	pCollection->Release();
	pEnumerator->Release();
	CoUninitialize();

	return microphoneArrayExists;
}


// 启用被禁用的 "Microphone Array" 设备
void EnableMicrophoneArray()
{
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pCollection = NULL;
	IAudioEndpointVolume *pEndpoint = nullptr;

	// 初始化 COM 库
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		cerr << "Failed to initialize COM library" << endl;
		return;
	}

	// 创建设备枚举器
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
	if (FAILED(hr))
	{
		cerr << "Failed to create device enumerator" << endl;
		CoUninitialize();
		return;
	}

	// 获取音频输入设备集合（包括禁用的）
	hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATEMASK_ALL, &pCollection);
	if (FAILED(hr))
	{
		cerr << "Failed to enumerate audio endpoints" << endl;
		pEnumerator->Release();
		CoUninitialize();
		return;
	}

	// 遍历音频输入设备
	UINT count;
	hr = pCollection->GetCount(&count);
	if (FAILED(hr))
	{
		cerr << "Failed to get device count" << endl;
		pCollection->Release();
		pEnumerator->Release();
		CoUninitialize();
		return;
	}

	bool microphoneArrayFound = false;

	// 查找 Microphone Array 设备并启用
	for (UINT i = 0; i < count; ++i)
	{
		IMMDevice *pDevice = NULL;
		hr = pCollection->Item(i, &pDevice);
		if (FAILED(hr))
		{
			cerr << "Failed to get device" << endl;
			continue;
		}

		LPWSTR pwszID = NULL;
		hr = pDevice->GetId(&pwszID);
		if (FAILED(hr))
		{
			cerr << "Failed to get device ID" << endl;
			pDevice->Release();
			continue;
		}

		if (pwszID != NULL)
		{
			// 获取设备的属性值
			IPropertyStore *pPropertyStore = NULL;
			hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
			if (SUCCEEDED(hr))
			{
				PROPVARIANT propvar;
				PropVariantInit(&propvar);

				// 读取设备的 FriendlyName 属性
				hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propvar);
				if (SUCCEEDED(hr))
				{
					wstring friendlyName(propvar.pwszVal);
					char deviceName[256];
					WideCharToMultiByte(CP_UTF8, 0, friendlyName.c_str(), -1, deviceName, 256, NULL, NULL);
					printf("Device: %s\n", deviceName);
					//printf("Test+++++++++++++++++++++++++++++end++++++++++: %s\n", friendlyName.find(L"Microphone Array"));
					if (friendlyName.find(L"Microphone Array") != wstring::npos)
					{
						microphoneArrayFound = true;
						//windows SDK API ---> AudioEndpointVolume 
						// windows SDK windowsUI

						// 启用设备
						hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **)&pEndpoint);

						if (SUCCEEDED(hr))
						{
							// 设备启用成功
							printf("Microphone Array device enabled: %ls\n", friendlyName.c_str());
							break;
						}
						else
						{
							// 设备启用失败
							printf("Failed to enable Microphone Array device: %ls\n", friendlyName.c_str());
						}
					}
				}

				PropVariantClear(&propvar);
				pPropertyStore->Release();
			}

			CoTaskMemFree(pwszID);
			pDevice->Release();
		}
	}

	if (!microphoneArrayFound)
	{
		cerr << "Microphone Array device not found or disabled." << endl;
	}

	pCollection->Release();
	pEnumerator->Release();
	CoUninitialize();
}





int main()
{
	OpenRecordingTab();

	// 检查并启用 Microphone Array 设备
	if (CheckMicrophoneArray())
	{
		printf("Microphone device is enabled and ready.\n");
	}
	else
	{
		printf("Microphone Array device not found or disabled. Enabling it now...\n");
		EnableMicrophoneArray();
		
	}

	return 0;
}
