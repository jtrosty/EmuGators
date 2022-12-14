#include "window.h"
#include "lights.h"
#include "weather.h"
#include "glwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

Window::Window(u32* _pixelData)
{
    WeatherManager *weather = new WeatherManager();
    weather->requestData();
    GLWidget* openGL = new GLWidget(this, _pixelData);
    openGL->weather = weather;
    mGLWidget = openGL;
    QPushButton *button = new QPushButton(this);
    button->setText("Change Weather");
    QLabel *weatherLabel = new QLabel(this);
    QObject::connect(weather, &WeatherManager::updated, weatherLabel, &QLabel::setText);

    LightManager* lights = new LightManager();
    openGL->lights = lights;
    QObject::connect(button, SIGNAL(pressed()), weather, SLOT(debug_cycleWeather()));
    QObject::connect(mGLWidget, SIGNAL(ghostDistance(int)), lights, SLOT(mirrorGhosts(int)));



    QGridLayout* layout = new QGridLayout(this);

    QKeyEvent* event;

    openGL->hasFocus();
    layout->addWidget(openGL, 0, 0);
    layout->addWidget(button, 0, 1);
    layout->addWidget(weatherLabel, 1, 1);
    setLayout(layout);


}
