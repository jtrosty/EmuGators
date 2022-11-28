#ifndef WEATHER_H
#define WEATHER_H

#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPainter>
#include <QRandomGenerator>

class WeatherManager : public QObject {
    Q_OBJECT

    QNetworkAccessManager nam;
    std::vector<QRectF> weatherTargets;

    int zipCode = 31014;
    QString currentWeather = "unknown";
    bool weatherChanged = false;

    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;

public:
    void setZip(int zip);
    int getZip();
    QString getWeather();
    void addWeatherEffect(QImage* img);
public slots:
    void requestData();
    void updateWeatherEffect();
    void debug_cycleWeather();
private slots:
    void httpFinished();
signals:
    void updated(QString weather);
};


#endif // WEATHER_H
