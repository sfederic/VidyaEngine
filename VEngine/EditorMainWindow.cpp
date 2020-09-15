#include "EditorMainWindow.h"
#include <qboxlayout.h>
#include <qtoolbutton.h>
#include <qdockwidget.h>
#include <qlistwidget.h>
#include "WorldWidget.h"
#include <qfilesystemmodel.h>
#include "RenderViewWidget.h"
#include "PropertiesWidget.h"
#include "CoreSystem.h"

EditorMainWindow::EditorMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //Toolbar dock
    toolbarDock = new ToolbarDock("Toolbar");
    addDockWidget(Qt::TopDockWidgetArea, toolbarDock);

    //World list
    worldDock = new WorldDock("World");
    addDockWidget(Qt::LeftDockWidgetArea, worldDock);

    //Properties dock
    propertiesDock = new PropertiesDock("Properties");
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock);

    //Asset dock
    assetDock = new AssetDock("Assets");
    addDockWidget(Qt::BottomDockWidgetArea, assetDock);

    //TODO: Console dock (tab it within asset/prefab window)

    //Central widget - Viewport
    {
        renderViewWidget = new RenderViewWidget();
        setCentralWidget(renderViewWidget);
        centralWidget()->setFixedSize(QSize(1000, 600));
        QSize size = centralWidget()->size();
        gCoreSystem.windowWidth = size.width();
        gCoreSystem.windowHeight = size.height();
    }
}

void EditorMainWindow::closeEvent(QCloseEvent* event)
{
    gCoreSystem.bMainLoop = false;
}

bool EditorMainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    MSG* msg = (MSG*)message;

    switch (msg->message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);

    case WM_KEYDOWN:
        gInputSystem.StoreKeyDownInput(msg->wParam);

        //Close editor
        if (msg->wParam == VK_ESCAPE)
        {

        }

        break;

    case WM_KEYUP:
        gInputSystem.StoreKeyUpInput(msg->wParam);
        break;

    case WM_LBUTTONUP:
        gInputSystem.StoreMouseLeftUpInput(msg->wParam);
        break;

    case WM_LBUTTONDOWN:
        gInputSystem.StoreMouseLeftDownInput(msg->wParam);
        break;

    case WM_RBUTTONUP:
        gInputSystem.StoreMouseRightUpInput(msg->wParam);
        break;

    case WM_RBUTTONDOWN:
        gInputSystem.StoreMouseRightDownInput(msg->wParam);
        break;

    case WM_MOUSEWHEEL:
        if (GET_WHEEL_DELTA_WPARAM(msg->wParam) < 0)
        {
            gInputSystem.StoreMouseWheelDown();
        }
        else
        {
            gInputSystem.StoreMouseWheelUp();
        }
        break;
    }

    return false;
}

