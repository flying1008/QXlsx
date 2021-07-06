// xlsxzipwriter.cpp

#include "xlsxzipwriter_p.h"

#include <QtGlobal>
#include <QDebug>
#include <QFile>

QT_BEGIN_NAMESPACE_XLSX

ZipWriter::ZipWriter(const QString &filePath)
{
#ifdef UNICODE
    std::wstring strPath = filePath.toStdWString();
    m_writer = CreateZip(strPath.c_str(),nullptr);
#else
    m_writer = CreateZip(strFile.toUtf8().constData(),nullptr);
#endif
}

ZipWriter::ZipWriter(QIODevice *device)
{
    QFile *file = dynamic_cast<QFile*>(device);
    if(nullptr != file)
    {
        QString strFile = file->fileName();
        file->close();
#ifdef UNICODE
    m_writer = CreateZip(reinterpret_cast<const wchar_t*>(strFile.utf16()),nullptr);
#else
    m_writer = CreateZip(strFile.toUtf8().constData(),nullptr);
#endif
    }
    else
    {
        m_writer =0;
    }
}
ZipWriter::~ZipWriter()
{
    CloseZip( m_writer);
}

bool ZipWriter::error() const
{
    return !IsZipHandleZ(m_writer);
}

void ZipWriter::addFile(const QString &filePath, QIODevice *device)
{
#ifdef UNICODE
    std::wstring strFilePath = filePath.toStdWString();
    ZipAdd(m_writer,strFilePath.c_str(),(void*)device->readAll().data(),device->size());
#else
    ZipAdd(m_writer,filePath.toUtf8().constData(),(void*)device->readAll().data(),device->size());
#endif

}

void ZipWriter::addFile(const QString &filePath, const QByteArray &data)
{
#ifdef UNICODE
    std::wstring strFilePath = filePath.toStdWString();
    ZipAdd(m_writer,strFilePath.c_str(),(void*)data.data(),data.size());
#else
    ZipAdd(m_writer,filePath.toUtf8().constData(),(void*)data.data(),data.size());
#endif

}

void ZipWriter::close()
{
    CloseZip( m_writer);
}

QT_END_NAMESPACE_XLSX
