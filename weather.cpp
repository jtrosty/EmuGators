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
    QNetworkRequest request = QNetworkRequest(QUrl("https://api.openweathermap.org/data/2.5/weather?zip=" + QString::number(zipCode) + "&appid=d5d57a296c8fc04f018024bfd7dccb75"));
    reply.reset(nam.get(request));
    connect(reply.get(), &QNetworkReply::finished, this, &WeatherManager::httpFinished);
}

void WeatherManager::updateWeatherEffect(){
    currentWeather = "Rain";
    emit updated(currentWeather);
    if(weatherTargets.empty()){
        if(currentWeather == "Clouds"){
            for(int i = 0; i < 6; i++){
                double randomFactor1 = QRandomGenerator::global()->generateDouble();
                double randomFactor2 = QRandomGenerator::global()->generateDouble();
                bool heads1 = QRandomGenerator::global()->bounded(2)==1;
                bool heads2 = QRandomGenerator::global()->bounded(2)==1;

                if(heads1){
                    randomFactor1 = randomFactor1 * -1;

                }
                if(heads2){
                    randomFactor2 = randomFactor2 * -1;

                }
                QRectF target = QRectF(-30 + 50 *i + 20 * randomFactor1,10 + 10*randomFactor2, 30, 30);
                weatherTargets.push_back(target);
            }
        }
        else if(currentWeather == "Rain"){
            for(int i = 0; i < 4; i++){
                QRectF target = QRectF(100*i, -100, 100, 100);
                weatherTargets.push_back(target);
                target = QRectF(100*i, 0, 100, 100);
                weatherTargets.push_back(target);
                target = QRectF(100*i, 100, 100, 100);
                weatherTargets.push_back(target);
                target = QRectF(100*i, 200, 100, 100);
                weatherTargets.push_back(target);
                target = QRectF(100*i, 300, 100, 100);
                weatherTargets.push_back(target);
            }
        }
    }
    else{

        if(currentWeather == "Clouds"){
            int windspeed = 5;
            for(int i = 0; i < weatherTargets.size(); i++){
                QRectF newTarget = weatherTargets[i].translated(windspeed, 0);
                if(newTarget.x() > 256 + 30){
                    newTarget.moveTo(-30, newTarget.y());
                }
                weatherTargets[i] = newTarget;
            }
        }
        else if(currentWeather == "Rain"){
            int rainspeed = 5;
            for(int i = 0; i < weatherTargets.size(); i++){
                QRectF newTarget = weatherTargets[i].translated(0, rainspeed);
                if(newTarget.y() > 240 + 100){
                    newTarget.moveTo(newTarget.x(), -100);
                }
                weatherTargets[i] = newTarget;
            }
        }
    }
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

    updateWeatherEffect();

    QImage weatherSprite;

    if(currentWeather == "Clouds"){
        weatherSprite = QImage(":/resources/images/test_cloud.png");
    }
    else if(currentWeather == "Rain"){
        weatherSprite = QImage(":/resources/images/test_rain_2.png");
    }

    for(QRectF target : weatherTargets){
        painter.drawImage(target, weatherSprite);
    }


}
