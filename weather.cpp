//need to create QTnetworkManager, supply with url, then read from QtNetworkReply into QJsonDocument
#include "weather.h"

void WeatherManager::setZip(int zip){
    zipCode = zip;

}
int WeatherManager::getZip(){
    return zipCode;

}
QString WeatherManager::getWeather(){
    return currentWeather;

}

void WeatherManager::requestData(){
    currentWeather = "unknown";
    QNetworkRequest request = QNetworkRequest(QUrl("https://api.openweathermap.org/data/2.5/weather?zip=" + QString::number(zipCode) + "&appid=d5d57a296c8fc04f018024bfd7dccb75"));
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

void WeatherManager::addWeatherEffect(QImage* img){
    if(img == nullptr)
        return;
    QPainter painter = QPainter(img);

    //branch based on current weather (rain, clouds, etc)

    //need to draw some pixelart using NES color palette or at least get some placeholders to implement
    //QImage weatherSprite(":/images/myImage.png");

    //painter.drawImage(target, image);
    //probably repeated somehow

}
