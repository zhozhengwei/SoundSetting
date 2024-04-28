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

// ���������õ� Recording ѡ�
void OpenRecordingTab()
{
	// ʹ�� WinExec �򿪿�������е���������
	WinExec("control mmsys.cpl,,1", SW_NORMAL);
}


// ����Ƿ���ڲ����� Microphone Array �豸
bool CheckMicrophoneArray()
{
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pCollection = NULL;
	IMMDevice *pDevice = NULL;
	IPropertyStore *pPropertyStore = NULL;

	// ��ʼ��COM��
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		cerr << "Failed to initialize COM library" << endl;
		return false;
	}

	// �����豸ö����
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
	if (FAILED(hr))
	{
		cerr << "Failed to create device enumerator" << endl;
		CoUninitialize();
		return false;
	}

	// ��ȡ��Ƶ�����豸����
	hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
	if (FAILED(hr))
	{
		cerr << "Failed to enumerate audio endpoints" << endl;
		pEnumerator->Release();
		CoUninitialize();
		return false;
	}

	// ������Ƶ�����豸
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


// ���ñ����õ� "Microphone Array" �豸
void EnableMicrophoneArray()
{
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDeviceCollection *pCollection = NULL;
	IAudioEndpointVolume *pEndpoint = nullptr;

	// ��ʼ�� COM ��
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		cerr << "Failed to initialize COM library" << endl;
		return;
	}

	// �����豸ö����
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
	if (FAILED(hr))
	{
		cerr << "Failed to create device enumerator" << endl;
		CoUninitialize();
		return;
	}

	// ��ȡ��Ƶ�����豸���ϣ��������õģ�
	hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATEMASK_ALL, &pCollection);
	if (FAILED(hr))
	{
		cerr << "Failed to enumerate audio endpoints" << endl;
		pEnumerator->Release();
		CoUninitialize();
		return;
	}

	// ������Ƶ�����豸
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

	// ���� Microphone Array �豸������
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
			// ��ȡ�豸������ֵ
			IPropertyStore *pPropertyStore = NULL;
			hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
			if (SUCCEEDED(hr))
			{
				PROPVARIANT propvar;
				PropVariantInit(&propvar);

				// ��ȡ�豸�� FriendlyName ����
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

						// �����豸
						hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **)&pEndpoint);

						if (SUCCEEDED(hr))
						{
							// �豸���óɹ�
							printf("Microphone Array device enabled: %ls\n", friendlyName.c_str());
							break;
						}
						else
						{
							// �豸����ʧ��
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

	// ��鲢���� Microphone Array �豸
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
