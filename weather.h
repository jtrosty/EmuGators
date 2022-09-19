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

class WeatherManager : public QObject {
    Q_OBJECT

    QNetworkAccessManager nam;
    bool raining;
    QString currentWeather;

    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;

public:
    bool isRaining();
    QString getWeather();
    void setup();
private slots:
    void httpFinished();
signals:
    void updated(QString weather);
};


#endif // WEATHER_H
