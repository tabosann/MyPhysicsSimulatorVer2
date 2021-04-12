#pragma once

//標準ライブラリ
#include<stdlib.h>
#include<string.h>
#include<iostream>

//並列処理ライブラリ
#include<mutex>
#include<thread>

//Windows標準ライブラリ
#include<Windows.h>
#include<wrl.h> //Microsoft::WRL::ComPtr

//DirectX12関連
#include<d3d12.h>
#include<dxgi1_6.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>

//DirectX12ヘルパーライブラリ
#include"../DirectXTex/d3dx12.h"
#include"../DirectXTex/DirectXTex.h"

//ImGui
#include"../ImGui/imgui.h"
#include"../ImGui/imgui_impl_win32.h"
#include"../ImGui/imgui_impl_dx12.h"

#pragma comment(lib, "DirectXTex.lib")

#define _USE_DEVELOP_VER_ false