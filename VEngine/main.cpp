#include "EditorMainWindow.h"
#include "RenderSystem.h"
#include "CoreSystem.h"
#include "UISystem.h"
#include "Obj.h"
#include "Camera.h"
#include "Audio.h"
#include "AudioSystem.h"
#include "Input.h"
#include "Actor.h"
#include "ShaderFactory.h"
#include "DebugMenu.h"
#include "Raycast.h"
#include "World.h"
#include "FileSystem.h"
#include "Debug.h"
#include "FBXImporter.h"
#include "WorldEditor.h"
#include "TimerSystem.h"
#include "MathHelpers.h"
#include "Console.h"
#include "Profiler.h"

#include "TestActor.h"

//For throwing the program into fullscreen for profilers, getting rid of Qt
//#define PROFILE_NOEDITOR

int main(int argc, char *argv[])
{
    HR(CoInitialize(NULL)); //For the WIC functions

    //Qt setup
#ifndef PROFILE_NOEDITOR
    QApplication qApplication(argc, argv);
    EditorMainWindow* editorMainWindow = new EditorMainWindow();
#endif //PROFILE_NOEDITOR
    gProfiler.Init();

    //FBX setup
    FBXImporter::Init();

    SetActiveCamera(&editorCamera);
    editorCamera.bEditorCamera = true;

    //Systems setup
    gCoreSystem.SetTimerFrequency();
#ifdef PROFILE_NOEDITOR
    gRenderSystem->Init(gCoreSystem.mainWindow);
#else
    gRenderSystem->Init((HWND)editorMainWindow->renderViewWidget->winId());
#endif
    gAudioSystem.Init();
    gUISystem.Init();
    gWorldEditor.Init();

    //Qt late init
    editorMainWindow->worldDock->PopulateWorldList();


    ActorSystem ac;
    ac.modelName = "cube.fbx";
    ac.shaderName = L"shaders.hlsl";
    ac.textureName = L"texture.png";
    ac.CreateActors<Actor>(gRenderSystem, 1);

    GetWorld()->AddActorSystem(ac);

    gRenderSystem->Flush();
    gRenderSystem->WaitForPreviousFrame();

    //MAIN LOOP
    while (gCoreSystem.bMainLoop)
    {
        const float deltaTime = gCoreSystem.deltaTime;

        gCoreSystem.StartTimer();
        gCoreSystem.HandleMessages();

        qApplication.processEvents();
        editorMainWindow->Tick();

        gFileSystem.Tick();
        gUISystem.Tick(editorMainWindow);

        gTimerSystem.Tick(deltaTime);

        GetActiveCamera()->Tick(deltaTime);

        gRenderSystem->Tick();
        gRenderSystem->RenderSetup(deltaTime);

        gWorldEditor.Tick(nullptr, editorMainWindow);

        gRenderSystem->Render(deltaTime);
        gRenderSystem->RenderEnd(deltaTime);

        //UI RENDERING
        if (gUISystem.bAllUIActive)
        {
            //gUISystem.d2dRenderTarget->BeginDraw();
            gConsole.Tick();
            gConsole.DrawViewItems();
            debugMenu.Tick(GetWorld(), deltaTime);
            //gUISystem.RenderAllUIViews();
            //gUISystem.d2dRenderTarget->EndDraw();
        }

        gRenderSystem->Flush();

        //PRESENT
        gRenderSystem->Present();

        gRenderSystem->WaitForPreviousFrame();

        gInputSystem.InputReset();
        gProfiler.Clean();

        gCoreSystem.EndTimer();
    }

    gUISystem.Cleanup();
    qApp->quit();

    return 0;
}
