#include "window.h"
#include "lights.h"
#include "weather.h"
#include "glwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

Window::Window()
{
    WeatherManager *weather = new WeatherManager();
    weather->requestData();
    GLWidget* openGL = new GLWidget(this);
    openGL->weather = weather;
    QPushButton *button = new QPushButton(this);
    button->setText("Push Me!");
    QLabel *weatherLabel = new QLabel(this);
    QObject::connect(weather, &WeatherManager::updated, weatherLabel, &QLabel::setText);

    LightManager *lights = new LightManager();
    lights->setMode(1);


    QGridLayout* layout = new QGridLayout(this);
    RomLoader* romLoader = new RomLoader();


    QKeyEvent* event;

    openGL->hasFocus();
    layout->addWidget(openGL, 0, 0);
    layout->addWidget(button, 0, 1);
    layout->addWidget(weatherLabel, 1, 1);
    setLayout(layout);
}
