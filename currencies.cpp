#include "currencies.h"
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qsslconfiguration.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtScript/QScriptEngine>
#include <QStringList>
#include <QScriptValueIterator>
#include <QJsonValue>
#include <QMapIterator>
#include <QMessageBox>
#include "mainwindow.h"

Currencies::Currencies()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));
    QUrl url("http://www.wingssystems.com/index.php/rest_server/all_authors_designs/format/json");
    QNetworkRequest request(url);
    request.setRawHeader( "User-Agent" , "Mozilla Firefox" );
    manager->get(request);
}

Currencies::~Currencies()
{
//    delete this;
}

void Currencies::requestReceived(QNetworkReply *reply)
{
    reply->deleteLater();
    if(reply->error() == QNetworkReply::NoError) {
        // Get the http status code
        int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (v >= 200 && v < 300) // Success
        {
            // Here we got the final reply
            QByteArray result = reply->readAll();
//            parse(result);
//            emit requestIsFinished();
        }
        else if (v >= 300 && v < 400) // Redirection
        {
            // Get the redirection url
            QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            // Because the redirection url can be relative,
            // we have to use the previous one to resolve it
            newUrl = reply->url().resolved(newUrl);

            QMessageBox msgBox;
            msgBox.setWindowTitle("title");
            msgBox.setText("Error! Please connect to the internet and try again.");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::No);
            if(msgBox.exec() == QMessageBox::Yes){
//              window->close();
            }

            return; // to keep the manager for the next request
        }
    }
    else
    {
        // Error
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Error: \"" + reply->errorString() + "\".\n Please connect to the internet and try again.");
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
        }
    }
    reply->manager()->deleteLater();

}

//void HttpRequest::parse(QByteArray result)
//{
//        QScriptValue sc;
//        QScriptEngine engine;
//        sc = engine.evaluate(QString("(" + QString(result) + ")")); // In new versions it may need to look like engine.evaluate("(" + QString(result) + ")");
////        qDebug() << sc.toString();
//        QMap<QString, QVariant> myMap = decodeInner(sc);
////        qDebug() << *authorSingularList;
//}

//        QMap<QString, QVariant> HttpRequest::decodeInner(QScriptValue object)
//        {
//            QMap<QString, QVariant> map;
//            QScriptValueIterator it(object);

//            while (it.hasNext()) {
//                it.next();
//                if (it.value().isArray()){

//                    map.insert(it.name(),QVariant(decodeInnerToList(it.value())));
//                }
//                else if (it.value().isNumber())
//                {
//                    map.insert(it.name(),QVariant(it.value().toNumber()));
//                }
//                else if (it.value().isString())
//                {
//                    if (it.name() == "Author_Name") {
//                        authorName = it.value().toString();
//                        authorSingularList->append(authorName);
////                        qDebug() << "authorname: " << authorName;
//                    }
//                     if (it.name() == "Design_id") {designsAuthors.insert(it.value().toString(), authorName);}
//                     if (it.name() == "Package_Name") {designsAuthors.insert(it.value().toString(), authorName);}
//                }
//                else if (it.value().isNull()){
//                    map.insert(it.name(),QVariant());}
//                else if(it.value().isObject()){
//                    map.insert(it.name(),QVariant(decodeInner(it.value())));}
//            }
//            return map;
//        }

//        QList<QVariant> HttpRequest::decodeInnerToList(QScriptValue arrayValue)
//        {
//            QList<QVariant> list;
//            QScriptValueIterator it(arrayValue);
//            while (it.hasNext()) {
//                it.next();
//                if (it.name() == "length")
//                    continue;

//                if (it.value().isArray())
//                    list.append(QVariant(decodeInnerToList(it.value())));
//                else if (it.value().isNumber())
//                    list.append(QVariant(it.value().toNumber()));
//                else if (it.value().isString())
//                {
//                    list.append(QVariant(it.value().toString()));
//                }
//                else if (it.value().isNull())
//                    list.append(QVariant());
//                else if(it.value().isObject())
//                    list.append(QVariant(decodeInner(it.value())));
//            }
////            qDebug() << designsAuthors;
//            return list;
//        }
