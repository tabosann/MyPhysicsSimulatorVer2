#pragma once

//�W�����C�u����
#include<stdlib.h>
#include<string.h>
#include<iostream>

//���񏈗����C�u����
#include<mutex>
#include<thread>

//Windows�W�����C�u����
#include<Windows.h>
#include<wrl.h> //Microsoft::WRL::ComPtr

//DirectX12�֘A
#include<d3d12.h>
#include<dxgi1_6.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>

//DirectX12�w���p�[���C�u����
#include"../DirectXTex/d3dx12.h"
#include"../DirectXTex/DirectXTex.h"

//ImGui
#include"../ImGui/imgui.h"
#include"../ImGui/imgui_impl_win32.h"
#include"../ImGui/imgui_impl_dx12.h"

#pragma comment(lib, "DirectXTex.lib")

#define _USE_DEVELOP_VER_ false