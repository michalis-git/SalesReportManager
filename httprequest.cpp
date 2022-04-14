#include "httprequest.h"
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
#include <QString>
#include "mainwindow.h"
#include <QFile>


HttpRequest::HttpRequest(MainWindow *mainwindow, QString urlString, QString directoryPath)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));
    QUrl url(urlString);
    QNetworkRequest request(url);
    request.setRawHeader( "User-Agent" , "Mozilla Firefox" );
    manager->get(request);
    authorSingularList = new QStringList;
    window = mainwindow;
    if (urlString.startsWith("http://currencies"))
    {
        currencyName = urlString.right(7).left(3);
        dirPath = directoryPath;
        urlOfRequest = urlString;
    }
}

HttpRequest::~HttpRequest()
{
//    delete this;
}

void HttpRequest::requestReceived(QNetworkReply *reply)
{
    reply->deleteLater();
    if(reply->error() == QNetworkReply::NoError) {
        // Get the http status code
        int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (v >= 200 && v < 300) // Success
        {
            // Here we got the final reply
            QByteArray result = reply->readAll();
            QString command(result);
            if(command.left(1) == "{")
            {
                parseJson(result);
            }
            else
            {
                QRegExp rx("((\\w|[.]|[-]|[ ])+(\\n))");

                QStringList list;
                int pos = 0;
                while ((pos = rx.indexIn(command, pos)) != -1) {
                    list << rx.cap(1).replace("\n", "");
                    pos += rx.matchedLength();
                }
                mlist = list;
            }

            emit requestIsFinished();

            if (urlOfRequest.startsWith("http://currencies"))
            {
                QString fullPath = dirPath + "/" + currencyName + ".txt";
                if (!QFile(fullPath).exists()) {
                    QFile exchangeRateFile(fullPath);
                    exchangeRateFile.open(QIODevice::WriteOnly | QIODevice::Text);
                    QTextStream outStream(&exchangeRateFile);
                    for (int i = 0; i < mlist.count(); i++)
                    {
                        QString listItem = (mlist)[i];
                        outStream << listItem + "\n";
                    }
                    exchangeRateFile.close();
                }
                else
                {
                    QFile exchangeRateFile(fullPath);
                    exchangeRateFile.open(QIODevice::Append | QIODevice::Text);
                    QTextStream outStream(&exchangeRateFile);
                    for (int i = 0; i < mlist.count(); i++)
                    {
                        QString listItem = (mlist)[i];
                        outStream << listItem + "\n";
                    }
                    exchangeRateFile.close();
                }
            }

        }
        else if (v >= 300 && v < 400) // Redirection
        {
            // Get the redirection url
            QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            // Because the redirection url can be relative,
            // we have to use the previous one to resolve it
            newUrl = reply->url().resolved(newUrl);

//            QNetworkAccessManager *manager = reply->manager();
//            QNetworkRequest redirection(newUrl);
//            QNetworkReply *newReply = manager->get(redirection);
//            QMessageBox::warning( window, "Warning", message );
            QMessageBox msgBox;
            msgBox.setWindowTitle("title");
            msgBox.setText("Error! Please connect to the internet and try again.");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::No);
            if(msgBox.exec() == QMessageBox::Yes){
              window->close();
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
          window->close();
        }
    }
    reply->manager()->deleteLater();

}

void HttpRequest::parseJson(QByteArray result)
{
        QScriptValue sc;
        QScriptEngine engine;
        sc = engine.evaluate(QString("(" + QString(result) + ")")); // In new versions it may need to look like engine.evaluate("(" + QString(result) + ")");
        QMap<QString, QVariant> myMap = decodeInner(sc);
}

        QMap<QString, QVariant> HttpRequest::decodeInner(QScriptValue object)
        {
            QMap<QString, QVariant> map;
            QScriptValueIterator it(object);

            QString designOrPackage;
            while (it.hasNext()) {
                it.next();
                if (it.value().isArray()){

                    map.insert(it.name(),QVariant(decodeInnerToList(it.value())));
                }
                else if (it.value().isNumber())
                {
                    map.insert(it.name(),QVariant(it.value().toNumber()));
                }
                else if (it.value().isString())
                {
                    if (it.name() == "Author_Name") {
                        authorName = it.value().toString();
                        authorSingularList->append(authorName);
                    }
                    if (it.name() == "Design_id")
                    {
                        designsAuthors.insert(it.value().toString(), authorName);
                        designOrPackage = it.value().toString();
//                        qDebug() << formatName(designOrPackage);
                    }
                    if (it.name() == "Package_Name")
                    {
                        designsAuthors.insert(it.value().toString(), authorName);
                        designOrPackage = it.value().toString();
//                        qDebug() << formatName(designOrPackage);
                    }
                    if (it.name() == "PFlag" || it.name() == "DFlag")
                    {
                        if (it.value().toInteger() == 2 || it.value().toInteger() == 3 || it.value().toInteger() == 6 || it.value().toInteger() == 7)
                        {
                            //exclusive
                            designsWingsPercentageMap.insert(designOrPackage, 0.4);
                        }
                        else
                        {
                            designsWingsPercentageMap.insert(designOrPackage, 0.5);
                        }
                    }
                }
                else if (it.value().isNull()){
                    map.insert(it.name(),QVariant());}
                else if(it.value().isObject()){
                    map.insert(it.name(),QVariant(decodeInner(it.value())));}
            }
//            foreach (QString key, designsWingsPercentageMap.keys())
//            {
//                qDebug() << key << ", " << designsWingsPercentageMap.value(key);
//            }
            return map;
        }

//        QString HttpRequest::formatName(QString name)
//        {
//           QString formatedName = name.left(name.length() - 8);
//           if (formatedName.right(1) == ("_"))
//           {
//               formatedName = formatedName.left(formatedName.length() - 1);
//               formatedName.append(" (Pack.) ");
//           }
//           return formatedName;
//        }


        QList<QVariant> HttpRequest::decodeInnerToList(QScriptValue arrayValue)
        {
            QList<QVariant> list;
            QScriptValueIterator it(arrayValue);
            while (it.hasNext()) {
                it.next();
                if (it.name() == "length")
                    continue;

                if (it.value().isArray())
                    list.append(QVariant(decodeInnerToList(it.value())));
                else if (it.value().isNumber())
                    list.append(QVariant(it.value().toNumber()));
                else if (it.value().isString())
                {
                    list.append(QVariant(it.value().toString()));
                }
                else if (it.value().isNull())
                    list.append(QVariant());
                else if(it.value().isObject())
                    list.append(QVariant(decodeInner(it.value())));
            }
            return list;
        }
