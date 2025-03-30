# GTA-Editor

Generic Tool for Automation (GTA) Editor

## Description

The Generic Tool for Automation (GTA) allows the creation, modification, execution and results reporting of test Procedures on ground Test Means.
The Tool has an Editor module, a Scheduler module and modules to handle Lab Test Means (Ex. Image recognition).

Bug Tracking and Feature Management are follow on JIRA : https://jira.airbus.corp/projects/I04DAIVYTE

WIKI : https://confluence.airbus.corp/display/11T37GENER/

The UserGuide is available at https://docs.google.com.rproxy.goskope.com/document/d/14kUQQtI6lTz1fI4S2Pwn9bpnTpgZi4gLZYfZk0hm_Qk/edit?usp=drive_web&ouid=100028131742056610226

## GitHub repository usage policy

GitHub is connected with JIRA project ( [https://jira.airbus.corp/projects/I04DAIVYTE](https://jira.airbus.corp/projects/I04DAIVYTE "https://jira.airbus.corp/projects/i04daivyte") )  
In order to enable this link, please name your branch with the jira issue name I04DAIVYTE-xxx (ex. I04DAIVYTE-789 )

### My work is featured by one issue

Create one branch named with the issue names I04DAIVYTE-xxx (replace 'xxx' by issue ID)

### My work is featured by more than one issue

Some issue are entangled or merged. I want to link them all in a single branch.  
create one branch named with an aggregation of the issue names, separate by hyphen '-' symbol.

example:  
    branch name = "I04DAIVYTE-101-I04DAIVYTE-67-I04DAIVYTE-704-I04DAIVYTE-812"  
    This branch manage issue ID 101, 67, 704 and 812 of the jira project I04DAIVYTE

### My work is featured by an US with/without multi subtask

A US can be done in one job or task by task, by a single or multi developer.  
  
If you work on a US with subtask, think about :  
  
- creating a main US branch, named with US issue name    
- creating branches for each task, and merge each of them into the main US branch using pull requests

## Development environment

### Prerequisites
* ServiceNow request for Visual Studio PRO (2022) : https://airbus.service-now.com/esc?id=sc_cat_item&sys_id=5b9d89a9db7c1c54276e1aaf29961988&package_sys_id=cbe109d24789591464a02821e36d4336
* Request a local admin account for your worksatation, see ServiceNow request here : https://airbus.service-now.com/esc?id=kb_article&table=kb_knowledge&sysparm_article=KB0079337&searchTerm=TA-WKST
* QT 5.7 : Version 64 bits **qt-opensource-windows-x86-msvc2015_64-5.7.0.exe** to download from https://download.qt.io/new_archive/qt/5.7/5.7.0/qt-opensource-windows-x86-msvc2015_64-5.7.0.exe
* [optionnal] Doxygen https://www.doxygen.org/

### Installation steps
* Install Visual Studio:

    During installation (or in Visual Studio Installer and click modify or when Visual Studio is open in Tools/GetTools and features)
	- select *Desktop development with C++* in **Workloads tab**
	- select *Universal windows develoment platforme*
	
	![imgInstallationVS1](/doc/readme_file/VS_install_02.png)
		
	- select search *msvc v143 - VS 2022 C++ x64/x86 Build Tools (dernière version)* in **Individual Components** tab
	
	![imgInstallationVS1](/doc/readme_file/MSVC143.PNG)
	
	- Select Software Development Kit (SDK) Windows 10
	

* Add Qt VS Tools extension on Visual Studio:
    ```
    Extensions > Manage Extensions > Search > QT Visual Studio Tools > Legacy Qt Visual Studio Tools and click Download
    ```
> [!IMPORTANT]
> Close and restart Visual Studio to finish the installation of the extension
  
* Add Qt Path to VS if not yet done by default *(for example C:\Qt\Qt5.7.0\5.7\msvc2015_64\bin)*:
    ```
    Extensions > Qt VS Tools > Options > Versions > add new Qt version
    ```
### Clone code
Clone the repository required

### First Solution/Project loading
* Solution and Projects
    The solution and the projects are configured to be universal regardless of the developer on the team.
    Please never modify it to stay as close to the configuration as possible
    ```
    Project>Retarget solution.
    ```
    ![imgSolutionProjectLoad](/doc/readme_file/LatestSDK.JPG)
    
    => never automatically commit / push *.vcxproj and *.sln files
    In case of necessary modification, make sure not to define an SDK target
    

### Compilation 
* [If not Done] Make sure to convert all Projects to QT Projects:
    ```
    Solution Explorer > Right click on selected project > Qt > convert custom build steps to Qt/MSbuild
    Note: it appears to be unnecessary, as building all the solutions works without doing it.
          (sometimes, the option "convert custom build..." does not even appear.)
    ```
* [If not Done] Make sure to select the right version of QT in every project:
    ```
    Solution Explorer > Right click on selected project > Qt > Qt Project Settings > Qt Installation
    Note: If the right QT version (as indicated) was installed, this step should not be necessary 
    ```
	![imgCompilation](/doc/readme_file/QT_Version.PNG)
* [If not Done] Make sure to select the right version of C++ 17 Standard in every project:
    ```
    Solution Explorer > Right click on selected project > properties > C/C++ > Langage > select *ISO C++17 Standard (/std:c++17)* in C++ Language Standard
    Note: If the right C++ Language standard 17 (as indicated) was installed, this step should not be necessary 
    ```
	![imgCompilation](/doc/readme_file/C++17.PNG)
	
* Projects build Order:
    ````
    1. GTAZip -> GTACommon -> GTAMath -> GTACommands -> GTADataModel -> GTAParsers
    2. GTACore -> GTACommandsSerializer -> GTAViewModel -> GTASCXML -> GTACMDSCXML
    3. GTAXmlRpcServerClient -> GTASequencer
    4. GTAControllers -> GTAUI -> GTAApp
    ````
    Remember that each successive solution builds on the previous ones. Therefore, every previous solution must be generated to build the next ones.

### Files and Libraries to add after successful build:
In order to use the application, you have to add these files to the *build -> debug* and *build -> release* folders :
<!-- # 1. GTA.xml and GTA_Generic.xml : These two files fills the comboboxes of GTA application.

    ![imgFileNLib01](/doc/readme_file/GTA_XML.png)	-->

<!-- 2. AppConfig.ini must be placed with the .exe of the application. This file is necessary to indicate the location of the bench configuration ini files.
-->
<!-- 3.--> IHM and Tool Data files : it contains the templates of different GTA files, pictures of the IHM as well as other tools
    
![imgFileNLib02](/doc/readme_file/Tool_Data.png)
    
<!-- 	
4. CLRZIP.dll : that is necessary for the application to run
    
    ![imgFileNLib03](/doc/readme_file/CLRZIP.png)
-->	
<!-- 5. QT libraries:

	i. Debug mode
	
    ![imgFileNLib04](/doc/readme_file/QT_Debug_mode.PNG)
	
	ii. Release mode
	
    ![imgFileNLib05](/doc/readme_file/QT_Release_mode.PNG)
-->

### Building package

Update then execute ```_Build_Package.bat```

## How to install *GTA-Editor*

### Dependencies

* Windows 7 or higher done

### Installing

Official packages can be found at https://artifactory.fr.eu.airbus.corp/ui/repos/tree/General/r-1t37-gta-generic-releases-local/GTA-Editor/official
* Download then unzip a package

* LaunchScxml.bat:
>- "EMOEXEPATH" to modify with your local emotest file path
>- "EMOCONFFILE" need to be set to default conf file of local emotest    
    **![imgBasicCfg01](/doc/readme_file/Emotest_path.png)**
	
	
* First Configuration of GTA:
    1. Bench database Path : add the path to your local Bench Database
    2. Procedure Repository Path : add the path to your workspace 
    3. Log Path : add the path to your workspace where files *(.log)* are generated
    4. Tool Data Path : add the path to the Tool Data folder 
	
	![imgSettings](/doc/readme_file/Settings.png)
	
 * Rebuild Database:
    That is the first thing to do in order to use GTA efficiently yes


### Executing program

Launch ```GTAApp.exe```

## Help
# GTA-Editor-New
# ram
great

Do not hesitate to contact dl-gta-dev@airbus.com
