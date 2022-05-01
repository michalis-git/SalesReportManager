#include "decompresser.h"
#include "qdir.h"
#include "zlib.h"

#include <QDataStream>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

QByteArray gzipDecompress(QByteArray &compressData ) {
  //strip header
  compressData.remove(0, 10);

  const int buffer_size = 16384;
  quint8 buffer[buffer_size];

  z_stream cmpr_stream;
  cmpr_stream.next_in = (unsigned char *)compressData.data();
  cmpr_stream.avail_in = compressData.size();
  cmpr_stream.total_in = 0;

  cmpr_stream.next_out = buffer;
  cmpr_stream.avail_out = buffer_size;
  cmpr_stream.total_out = 0;

  cmpr_stream.zalloc = Z_NULL;
  cmpr_stream.zfree = Z_NULL;
  cmpr_stream.opaque = Z_NULL;

  int status = inflateInit2( &cmpr_stream, -8 );
  if (status != Z_OK) {
    qDebug() << "cmpr_stream error!";
  }

  QByteArray uncompressed;
  do {
    cmpr_stream.next_out = buffer;
    cmpr_stream.avail_out = buffer_size;

    status = inflate( &cmpr_stream, Z_NO_FLUSH );

    if (status == Z_OK || status == Z_STREAM_END) {
      QByteArray chunk = QByteArray::fromRawData((char *)buffer, buffer_size - cmpr_stream.avail_out);
      uncompressed.append( chunk );
    } else {
      inflateEnd(&cmpr_stream);
      break;
    }

    if (status == Z_STREAM_END) {
      inflateEnd(&cmpr_stream);
      break;
    }
  }
  while (cmpr_stream.avail_out == 0);

  return uncompressed;
}


QString decompressedFilePathFromPath(const QString &decompressedDirpath, const QString &compressedFilePath) {
    QString fileName = QFileInfo(compressedFilePath).fileName();
    // removes 'gz'
    fileName = fileName.left(fileName.lastIndexOf("."));
    QString decompressedFilePath = decompressedDirpath + QDir::separator() + fileName;
    return decompressedFilePath;
}

Decompresser::Decompresser(const QString &compressedFilepath, const QString &decompressedDirpath) {
    QByteArray compressedData;
    if (!readCompressedData(compressedFilepath, compressedData))
            qDebug() << "Error: unable to read compressed file";
    else {
        if (!QDir(decompressedDirpath).exists()) {
          QDir().mkdir(decompressedDirpath);
        }
        QString decompFilePath = decompressedFilePathFromPath(decompressedDirpath,
                                                            compressedFilepath);
        if (!writeDecompressedData(decompFilePath, compressedData))
            qDebug() << "Error: unable to write decompressed file";
    }
}

const QString &Decompresser::decompressedFilePath() const {
    return m_decompressedFilePath;
}

bool Decompresser::readCompressedData(const QString &compressedFilePath, QByteArray &compressedData) {
    bool success = false;
    QFile sourceFile(compressedFilePath);
    if(sourceFile.open(QIODevice::ReadOnly)) {
      QDataStream sourceStream(&sourceFile);
      while(!sourceStream.atEnd()) {
        char *c = new char[1];
        sourceStream.readRawData(c,1);
        compressedData.push_back(*c);
        delete []c;
      }
      sourceFile.close();
      success = true;
    }
    return success;
}

bool Decompresser::writeDecompressedData(const QString &decompressedFilePath, QByteArray &data) {
    bool success = false;
    QFile destinationFile(decompressedFilePath);
    QByteArray destinationData = gzipDecompress(data);
    if (destinationFile.open(QIODevice::WriteOnly)) {
        success = true;
        destinationFile.write(destinationData);
        destinationFile.close();
        m_decompressedFilePath = decompressedFilePath;
    }
    return success;
}
