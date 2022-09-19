//need to create QTnetworkManager, supply with url, then read from QtNetworkReply into QJsonDocument
#include "weather.h"

bool WeatherManager::isRaining(){
    return raining;

}
QString WeatherManager::getWeather(){
    return currentWeather;

}

void WeatherManager::setup(){
    currentWeather = "unknown";
    QNetworkRequest request = QNetworkRequest(QUrl("https://api.openweathermap.org/data/2.5/weather?q=London&appid=d5d57a296c8fc04f018024bfd7dccb75"));
    reply.reset(nam.get(request));
    connect(reply.get(), &QNetworkReply::finished, this, &WeatherManager::httpFinished);
}

void WeatherManager::httpFinished(){
    QByteArray response_data = reply->readAll();
    QJsonDocument json = QJsonDocument::fromJson(response_data);

    QJsonObject root = json.object();
    QJsonArray weatherArray = root.value("weather").toArray();
    QJsonObject weatherObject = weatherArray[0].toObject();
    currentWeather = weatherObject.value("main").toString();
    emit updated(currentWeather);

}
