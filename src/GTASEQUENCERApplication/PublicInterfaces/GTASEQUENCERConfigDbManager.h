#ifndef AINGTASEQUENCERCONFIGDBMANAGER_H
#define AINGTASEQUENCERCONFIGDBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#define USER_DATA_DATABASE "USER_DATA_DB"
#define CONFIGURATION_TABLE "CONFIGURATION_TABLE"

class AINGTASEQUENCERConfigDbManager
{
public:
    AINGTASEQUENCERConfigDbManager();

    /**
      * Author: Priyanka
      * This function saves the list of tools/scripts , IP, configuration in database
      * @configName:  configuration name
      * @lstScripts:  list of scripts to be saved as QString appended with "," in DB
      * @lstTools: list of tools to be saved as QString appended with "," in DB
      * @IPConnected: IP on which server starts
      */
    void saveConfiguration(const QString &configName, const QStringList &lstScripts, const QStringList &lstTools, const QString &IPConnected);

    /**
      * Author: Priyanka
      * This function creates the database for saving configurations if one doesn't exist.
      */
    void createDatabase();
    void closeDatabase();

    /**
      * Author: Priyanka
      * This function adds new configuration column in Database
      */
    void addNewConfig(const QString &configName, const QStringList &lstScripts, const QStringList &lstTools, const QString &IPConnected);

    /**
      * Author: Priyanka
      * This function returns all the configurations names present in Database
      */
    QStringList getConfigNames();

    /**
      * Author: Priyanka
      * This function deletes configuration columns in Database
      * @iSelectedItems: list of configurations to be deleted.
      */
    void deleteConfigs(const QStringList &iSelectedItems);

    /**
      * Author: Priyanka
      * This function returns the list of scripts/tools and IP for particular configuration
      * @iConfigName: configurtaion name.
      * @return: QStringList with elements list of scripts(appended with ','), list of tools(appended with ',') and iP.
      */
    QStringList getConfigFiles(const QString &iConfigName);
private:
    QSqlDatabase _UserDb;
    QString _dbPath;
    bool openUserDatabase(const QString &iFile);
};

#endif // AINGTASEQUENCERCONFIGDBMANAGER_H
