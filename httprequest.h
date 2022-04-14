#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "qobject.h"
#include <qmap.h>
#include <QStringList>

class QNetworkReply;
class QScriptValue;
//class QStringList;
class MainWindow;

class HttpRequest : public QObject
{
    Q_OBJECT

public:
    HttpRequest(MainWindow* mainwindow, QString url, QString directoryPath);
    ~HttpRequest();
    QMap<QString, QString> designsAuthors;
    QMap<QString, float> designsWingsPercentageMap;
    QString authorName, designName;
    QStringList* authorSingularList;
    QStringList mlist;
    QString currencyName;
    QString dirPath;
    QString urlOfRequest;

private:
    MainWindow* window;
    void parseJson(QByteArray result);
    QMap<QString, QVariant> decodeInner(QScriptValue object);
    QList<QVariant> decodeInnerToList(QScriptValue arrayValue);
//    QString formatName(QString name);

private slots:
    void requestReceived(QNetworkReply *reply);

signals:
    void requestIsFinished();
    void rateRequestIsFinished(QString currencyName);
};

#endif // HTTPREQUEST_H
