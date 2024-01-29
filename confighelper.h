#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QObject>
#include <QString>
#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>

#include <math.h>
#include <string.h>


class ConfigHelper : public QObject
{
    Q_OBJECT

    QString filepath;
    QFile *file;
    QJsonObject root;


public:
    explicit ConfigHelper(QObject *parent = nullptr) {
        #ifdef __WIN32__
        filepath = "C:\\Users\\Sam\\Desktop\\CpConsole_Data\\device_config.json";
        #elif __linux__
        filepath = "/boot/firmware/device_config.json";
        #endif

        file = new QFile(filepath);

        file->open(QIODevice::ReadOnly);
        QByteArray buffer = file->readAll();
        file->close();

        QJsonDocument doc = QJsonDocument::fromJson(buffer);

        // Check if document is empty
        if (doc.isEmpty()) {
            // Create new root object
            qDebug() << "Config file is empty. Creating new config.";
            root = createBaseConfig();
            doc.setObject(root);

            // Write new document to file
            qDebug() << "Writing config to file.";
            file->open(QIODevice::WriteOnly | QIODevice::Truncate);
            buffer = doc.toJson();

            file->write(buffer);
            file->close();
        }
        else {
            // Load root object from document
            root = doc.object();
        }
    }

    QJsonValue getParameter(QString key) {
        QJsonValue raw_val = root.value(key);

        // No need to parse value if its not a string
        if (!raw_val.isString()) {
            return raw_val;
        }

        // Parse string
        size_t index = 0;
        std::string cstr = raw_val.toString().toStdString();

        while (index != std::variant_npos) {
            index = 0;

            // Find open tag
            index = cstr.find("{{", index);
            if (index == std::variant_npos) continue;
            size_t first = index;   // Increment index so we strip the tag

            // Find close tag
            index = cstr.find("}}", index);
            if (index == std::variant_npos) continue;
            size_t last = index + 2;        // Dont increment index so we strip the tag

            // Get element name
            std::string name = cstr.substr(first+2, (last-2)-(first+2));

            if (strcmp(name.c_str(), "console_id") == 0) {
                int console_id = root.value("console_id").toInt();

                // Convert integer to string
                char *console_id_str = (char *) malloc(sizeof(char) * (int) log10(console_id));
                sprintf(console_id_str, "%d", console_id);

                cstr.replace(first, last-first, console_id_str);

                // Make sure we free the allocated memory from before, no mem leaks here ;)
                free(console_id_str);
                continue;
            }
        }

        // Return value
        return QJsonValue(cstr.c_str());
    }

    void setParameter(QString key, QJsonValue val) {
        root[key] = val;
    }

    void saveConfig() {
        // Write new document to file
        file->open(QIODevice::WriteOnly | QIODevice::Truncate);

        QJsonDocument doc;
        doc.setObject(root);

        QByteArray buffer = doc.toJson();

        file->write(buffer);
        file->close();
    }

    QJsonObject createBaseConfig() {
        QJsonObject obj;

        obj.insert("console_id", 1);

        obj.insert("osc_host", "10.0.0.10");
        obj.insert("osc_host_port", 53000);
        obj.insert("osc_port", 54000);

        obj.insert("button_h_path", "/console/{{console_id}}/h");
        obj.insert("button_c_path", "/console/{{console_id}}/c");
        obj.insert("button_b_path", "/console/{{console_id}}/b");

        obj.insert("ndi_stream", "LAPTOP-8L7MPOSJ (Test Pattern)");

        return obj;
    }

signals:
};

#endif // CONFIGHELPER_H
