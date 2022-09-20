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

class WeatherManager : public QObject {
    Q_OBJECT

    QNetworkAccessManager nam;

    int zipCode = 32608;
    QString currentWeather;

    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;

public:
    void setZip(int zip);
    int getZip();
    QString getWeather();
    void addWeatherEffect(QImage* img);
public slots:
    void requestData();
private slots:
    void httpFinished();
signals:
    void updated(QString weather);
};


#endif // WEATHER_H
