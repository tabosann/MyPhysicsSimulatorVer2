# MyPhysicsSimulatorVer2

Let me apologize for my poor English.
I have been creating the MyPhysics Engine for 7 months by myself.

## How to use
1. Supervisor Menu in Edit can delete selected item by right-click on it and change parameters of item.
2. Creator Menu in Edit can generate item for pushback and delete it for popback.
3. Debug Console Menu in Setting can change camera position, light direction, back groung color and field of view.
4. Show Demo Menu in Setting shows demo window for ImGui. This is useful to understand how we use ImGui.
5. Play / Stop switch can play and stop animation.
6. All Reset button can reset present parameters to first ones.

From 1 to 4 Menues is around left-top corner and From 5 to 6 buttons is around right-top corner. Example below.
![衝突テストver2 2021_03_30 1_04_29](https://user-images.githubusercontent.com/75087682/112865846-23aea480-90f4-11eb-9131-46c06d390820.png)

## Introductory Images
![紹介画像１](https://user-images.githubusercontent.com/75087682/112883851-bdcd1780-9109-11eb-81f7-4934c876ecb8.png)
![紹介画像２](https://user-images.githubusercontent.com/75087682/112884087-0c7ab180-910a-11eb-9764-b1c679020789.png)

## Setting
This engine uses below under win32.

- DX12
- Dear ImGui
- DirectXTex

And this engine runs under below setting.

- DirectXTex_Desktop_2019_Win10.sln have run.
- imgui_impl_win32.h / .cpp
- imgui_impl_dx12.h / .cpp

This engine uses files that ImGui offers, they are in ImGui folder, and d3dx12.h that DirectXTex offers.
I uses DirectXTex project under DirectXTex_Desktop_2019_Win10.sln. For more details, refer to https://github.com/microsoft/DirectXTex.

## Existing Bugs
Bugs which I want to fix are described by commenting FIXME. So, you can find FIXME comments by searching.(Ctrl + F in the documents)
Appear bugs which I find out below.
- Source code that creates unique item name runs incorrectly in Application::ShowSupervisorMenu() implementation in Application.cpp.
- I want to realize collision calc for bounding sphere more accurately. In BS3.cpp file.
- In ObjectRenderer.cpp file, my original Descriptor Tables cannot be set to RootSignature. Use helpers of d3dx12.h now.
- Collision calc in Application::Run() in Application.cpp is not runs correctly.
