# MyPhysicsSimulatorVer2
Let me apologize for my poor English. 
## Setting
I have been creating the MyPhysics Engine for 7 months by myself.
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

## How to use
1. Supervisor Menu in Edit can delete selected item by right-click on it and change parameters of item.
2. Creator Menu in Edit can generate item for pushback and delete it for popback.
3. Debug Console Menu in Setting can change camera position, light direction, back groung color and field of view.
4. Show Demo Menu in Setting shows demo window for ImGui. This is useful to understand how we use ImGui.

From 1 to 4 Menues is around left-top corner. Example below.
![衝突テストver2 2021_03_30 1_04_29](https://user-images.githubusercontent.com/75087682/112865846-23aea480-90f4-11eb-9131-46c06d390820.png)

5. Play / Stop switch can play and stop animation.
6. All Reset button can reset present parameters to first ones.
