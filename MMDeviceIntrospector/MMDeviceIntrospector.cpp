// MMDeviceIntrospector.cpp : Test case for how SoundBlaster X3/4 SPDIF Out reports its speaker configuration to Windows
// wstephenson 2021

#include <initguid.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

#include <iostream>

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

int main()
{
    HRESULT hr = S_OK;

    IMMDeviceEnumerator* pDevEnum = NULL;
    IMMDevice* pDefaultDev = NULL;
    IPropertyStore* pProps = NULL;
    LPWSTR pwszID = NULL;
    PROPVARIANT varName, varPhysicalSpeakers;
    PropVariantInit(&varName);
    PropVariantInit(&varPhysicalSpeakers);

    hr = CoInitialize(NULL);
    EXIT_ON_ERROR(hr);
    
    hr = CoCreateInstance(
        CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, IID_IMMDeviceEnumerator,
        (void**)&pDevEnum);
    EXIT_ON_ERROR(hr);

    hr = pDevEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDev);
    EXIT_ON_ERROR(hr);

    // Get the endpoint ID string.
    hr = pDefaultDev->GetId(&pwszID);
    EXIT_ON_ERROR(hr);

    hr = pDefaultDev->OpenPropertyStore(
        STGM_READ, &pProps);
    EXIT_ON_ERROR(hr);

    // Get the endpoint's friendly-name property.
    hr = pProps->GetValue(
        PKEY_Device_FriendlyName, &varName);
    EXIT_ON_ERROR(hr);

    // Get the endpoint's physical speaker property.
    hr = pProps->GetValue(
        PKEY_AudioEndpoint_PhysicalSpeakers,
        &varPhysicalSpeakers);
    EXIT_ON_ERROR(hr);

    // Print endpoint friendly name and speaker property (as hex string)
    printf("Endpoint '%S' (%S) has PKEY_AudioEndpoint_PhysicalSpeakers: 0x%x\n",
        varName.pwszVal, pwszID, varPhysicalSpeakers.uintVal);
    printf("To find the meaning of this channel bitmask, see:\n");
    printf("https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ksmedia/ns-ksmedia-ksaudio_channel_config\n");
    printf("TL;DR: 0x3 is stereo, 0x3f is 5.1, 0x63f is 7.1\n");

    goto Cleanup;

Exit:
    printf("Error!\n");
Cleanup:
    CoTaskMemFree(pwszID);
    pwszID = NULL;
    PropVariantClear(&varName);
    PropVariantClear(&varPhysicalSpeakers);
    SAFE_RELEASE(pProps);
    SAFE_RELEASE(pDefaultDev);
    SAFE_RELEASE(pDevEnum);
    exit(HRESULT_CODE(hr));
}