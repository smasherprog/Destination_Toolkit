#ifndef MY_UI_TEST_H
#define MY_UI_TEST_H

#pragma warning(push)
#pragma warning( disable : 4005 )
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#pragma warning(pop)
extern HWND						g_pHWND;
extern ID3D11Device				*g_Device;
extern IDXGISwapChain			*SwapChain;
extern ID3D11RenderTargetView	*BackBufferRTV;
extern ID3D11Texture2D			*BackBuffer;
extern ID3D11DeviceContext		*g_DeviceContext;


void Init();
void InitLoginScreen();
void DeInitLoginScreen();
void DeInit();

void SaveInfo();
void LoadInfo();

void CheckCredentials();

#endif