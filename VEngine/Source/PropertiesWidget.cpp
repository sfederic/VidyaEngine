#include "PropertiesWidget.h"
#include "WorldEditor.h"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include "MathHelpers.h"
#include "../EditorMainWindow.h"
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include "RenderViewWidget.h"
#include "MathHelpers.h"
#include <qscrollarea.h>
#include "PropertyWidgets/BoolWidget.h"
#include <qscrollarea.h>
#include "CommandSystem.h"
#include "Commands/VectorCommand.h"
#include "PropertyWidgets/FloatWidget.h"

PropertiesWidget::PropertiesWidget(QWidget* parent) : QWidget(parent)
{
    QGridLayout* grid = new QGridLayout();

    //Position
    QLabel* posLabel = new QLabel("Position");

    //TODO: this all needs to change and get created on actor clicks in the viewport 
    //so that the undo system will work with pointer refs into the TransformEditWidgets.

    //QDoubleSpinBox::editingFinished calls its signal function twice for some reason.
    posEditX = new FloatWidget(0.f, this);
    connect(posEditX, &FloatWidget::editingFinished, this, &PropertiesWidget::SetActorPosition);
    posEditY = new FloatWidget(0.f, this);
    connect(posEditY, &FloatWidget::editingFinished, this, &PropertiesWidget::SetActorPosition);
    posEditZ = new FloatWidget(0.f, this);
    connect(posEditZ, &FloatWidget::editingFinished, this, &PropertiesWidget::SetActorPosition);

    QLabel* xLabel = new QLabel("X");
    xLabel->setStyleSheet("border: 1px solid red;");
    QLabel* yLabel = new QLabel("Y");
    yLabel->setStyleSheet("border: 1px solid green;");
    QLabel* zLabel = new QLabel("Z");
    zLabel->setStyleSheet("border: 1px solid blue;");

    grid->addWidget(xLabel, 0, 1);
    grid->addWidget(yLabel, 0, 2);
    grid->addWidget(zLabel, 0, 3);

    grid->addWidget(posLabel, 1, 0);
    grid->addWidget(posEditX, 1, 1);
    grid->addWidget(posEditY, 1, 2);
    grid->addWidget(posEditZ, 1, 3);

    //Rotation
    rotEditX = new FloatWidget(0.0f, this);
    connect(rotEditX, &FloatWidget::editingFinished, this, &PropertiesWidget::SetActorRotation);
    rotEditY = new FloatWidget(0.0f, this);
    connect(rotEditY, &FloatWidget::editingFinished, this, &PropertiesWidget::SetActorRotation);
    rotEditZ = new FloatWidget(0.0f, this);
    connect(rotEditZ, &FloatWidget::editingFinished, this, &PropertiesWidget::SetActorRotation);

    QLabel* rotLabel = new QLabel("Rotation");

    grid->addWidget(rotLabel, 2, 0);
    grid->addWidget(rotEditX, 2, 1);
    grid->addWidget(rotEditY, 2, 2);
    grid->addWidget(rotEditZ, 2, 3);

    //Scale
    scaleEditX = new FloatWidget(0.0f, this);
    connect(scaleEditX, &QDoubleSpinBox::editingFinished, this, &PropertiesWidget::SetActorScale);
    scaleEditY = new FloatWidget(0.0f, this);
    connect(scaleEditY, &QDoubleSpinBox::editingFinished, this, &PropertiesWidget::SetActorScale);
    scaleEditZ = new FloatWidget(0.0f, this);
    connect(scaleEditZ, &QDoubleSpinBox::editingFinished, this, &PropertiesWidget::SetActorScale);

    QLabel* scaleLabel = new QLabel("Scale");

    grid->addWidget(scaleLabel, 3, 0);
    grid->addWidget(scaleEditX, 3, 1);
    grid->addWidget(scaleEditY, 3, 2);
    grid->addWidget(scaleEditZ, 3, 3);

    grid->setAlignment(Qt::AlignTop);

    QVBoxLayout* vLayoutTop = new QVBoxLayout();

    vLayoutTop->addLayout(grid);

    //Actor/ActorSystem Properties
    //Since every actor is going to have these, make this one widget and then
    //throw any other inherited actor properties below this.
    QGridLayout* actorSystemDetailsGrid = new QGridLayout();

    QGridLayout* actorDetailsGrid = new QGridLayout();

    QCheckBox* checkBox = new QCheckBox();
    checkBox->setText("Renderable");
    checkBox->setChecked(true);
    actorSystemDetailsGrid->addWidget(checkBox, 0, 0);
    connect(checkBox, &QCheckBox::clicked, this, &PropertiesWidget::SetActorRenderable);

    //Actor properties
    actorDetailsGrid->addWidget(new QLabel("Actor Name"), 0, 0);
    actorName = new QLineEdit();
    connect(actorName, &QLineEdit::editingFinished, this, &PropertiesWidget::SetActorName);
    actorDetailsGrid->addWidget(actorName, 0, 1);

    //Actorsystem properties
    actorSystemName = new QLineEdit();
    actorSystemName->setReadOnly(true);
    actorSystemDetailsGrid->addWidget(actorSystemName, 1, 0);

    actorSystemModelName = new QLineEdit();
    actorSystemModelName->setReadOnly(true);
    actorSystemDetailsGrid->addWidget(actorSystemModelName, 2, 0);

    actorSystemShaderName = new QLineEdit();
    actorSystemShaderName->setReadOnly(true);
    actorSystemDetailsGrid->addWidget(actorSystemShaderName, 3, 0);

    actorSystemTextureName = new QLineEdit();
    actorSystemTextureName->setReadOnly(true);
    actorSystemDetailsGrid->addWidget(actorSystemTextureName, 4, 0);

    //Open file dialog buttons (These are fine to be defined locally like this)
    QPushButton* selectModelButton = new QPushButton("Select Model");
    connect(selectModelButton, &QPushButton::pressed, this, &PropertiesWidget::SelectModel);
    actorSystemDetailsGrid->addWidget(selectModelButton, 2, 1);

    QPushButton* selectShaderButton = new QPushButton("Select Shader");
    connect(selectShaderButton, &QPushButton::pressed, this, &PropertiesWidget::SelectShader);
    actorSystemDetailsGrid->addWidget(selectShaderButton, 3, 1);

    QPushButton* selectTextureButton = new QPushButton("Select Texture");
    connect(selectTextureButton, &QPushButton::pressed, this, &PropertiesWidget::SelectTexture);
    actorSystemDetailsGrid->addWidget(selectTextureButton, 4, 1);

    vLayoutTop->addLayout(actorDetailsGrid);
    vLayoutTop->addLayout(actorSystemDetailsGrid);

    entireVLayout = new QVBoxLayout();
    entireVLayout->addLayout(vLayoutTop);

    setLayout(entireVLayout);
}

void PropertiesWidget::SetActorPosition()
{
    Actor* picked = gWorldEditor.pickedActor;
    if (picked)
    {
        XMFLOAT3 newPos = {};
        newPos.x = posEditX->value();
        newPos.y = posEditY->value();
        newPos.z = posEditZ->value();

        gCommandSystem.AddCommand(new VectorCommand(newPos, &picked->transform.position));

        gWorldEditor.pickedActor->SetPosition(newPos);
    }
}

void PropertiesWidget::SetActorScale()
{
    Actor* picked = gWorldEditor.pickedActor;
    if (picked)
    {
        XMFLOAT3 newScale = {};
        newScale.x = scaleEditX->value();
        newScale.y = scaleEditY->value();
        newScale.z = scaleEditZ->value();

        gCommandSystem.AddCommand(new VectorCommand(newScale, &picked->transform.scale));

        gWorldEditor.pickedActor->SetScale(newScale);
    }
}

void PropertiesWidget::SetActorRotation()
{
    Actor* picked = gWorldEditor.pickedActor;
    if (picked)
    {
        XMFLOAT3 newRot = {};
        newRot.x = rotEditX->value();
        newRot.y = rotEditY->value();
        newRot.z = rotEditZ->value();

        gWorldEditor.pickedActor->SetRotation(newRot);
    }
}

void PropertiesWidget::SetActorName()
{
    gWorldEditor.pickedActor->name = actorName->text().toStdString();
    gEditorSystem->PopulateWorldList();
}

void PropertiesWidget::SelectShader()
{
    if (selectedActorSystem == nullptr)
    {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Shaders"), "Shaders", tr("Shader Files (*.hlsl)"));

    if (!filePath.isEmpty())
    {
        QString fileName = QFileInfo(filePath).fileName();
        actorSystemShaderName->setText(QFileInfo(fileName).fileName());

        selectedActorSystem->shaderName = fileName.toStdString();
    }
}

void PropertiesWidget::SelectModel()
{
    if (selectedActorSystem == nullptr)
    {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, 
        tr("Open Models"), "Models", tr("Model Files (*.fbx *.obj)"));

    if (!filePath.isEmpty())
    {
        QString fileName = QFileInfo(filePath).fileName();
        actorSystemModelName->setText(fileName);

        selectedActorSystem->modelName = fileName.toStdString();
        selectedActorSystem->RecreateModel();
    }
}

void PropertiesWidget::SelectTexture()
{
    if (selectedActorSystem == nullptr)
    {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Textures"), "Textures", tr("Texture Files (*.jpg *.png)"));

    if (!filePath.isEmpty())
    {
        QString fileName = QFileInfo(filePath).fileName();
        actorSystemTextureName->setText(fileName);

        selectedActorSystem->textureName = fileName.toStdString();
        selectedActorSystem->RecreateTexture();
    }
}

void PropertiesWidget::Tick()
{
    Actor* picked = gWorldEditor.pickedActor;
    if (picked)
    {
        XMFLOAT3 pos = picked->GetPositionFloat3();
        posEditX->setValue(pos.x);
        posEditY->setValue(pos.y);
        posEditZ->setValue(pos.z);

        XMFLOAT3 rot = PitchYawRollFromMatrix(picked->GetTransformationMatrix());
        rotEditX->setValue(XMConvertToDegrees(rot.x));
        rotEditY->setValue(XMConvertToDegrees(rot.y));
        rotEditZ->setValue(XMConvertToDegrees(rot.z));

        XMFLOAT3 scale = picked->GetScale();
        scaleEditX->setValue(scale.x);
        scaleEditY->setValue(scale.y);
        scaleEditZ->setValue(scale.z);
    }
}

void PropertiesWidget::SetActorRenderable(bool renderCheck)
{
    if (selectedActorSystem == nullptr)
    {
        return;
    }

    selectedActorSystem->GetActor(0)->bRender = renderCheck;
}
