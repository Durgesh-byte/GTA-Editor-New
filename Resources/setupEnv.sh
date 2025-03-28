echo Hello
if [[ -z "${AIRBUSID}" ]]; then AIRBUSID=$USERNAME; fi
echo Please now type your password, It will be hidden, but do not worry it is writing
read -s pwd
cd ..
cd ..

eMoTestScheduler="eMoTest_Scheduler_v1.2.8.2"
curl --insecure -u$AIRBUSID:$pwd -O "https://artifactory.fr.eu.airbus.corp/artifactory/r-1t37-gta-generic-releases-local/GTA-Editor/develop/00-Env-automatic-deploy/$eMoTestScheduler.zip"
unzip $eMoTestScheduler
rm "$eMoTestScheduler.zip"

gtaEnv="GTA-BasicEnv"
curl --insecure -u$AIRBUSID:$pwd -O "https://artifactory.fr.eu.airbus.corp/artifactory/r-1t37-gta-generic-releases-local/GTA-Editor/develop/00-Env-automatic-deploy/$gtaEnv.zip"
unzip $gtaEnv
rm "$gtaEnv.zip"
clear
DIR="GTA-BasicEnv"
if [ -d "$DIR" ]; then
  echo "Setup Finished"
echo "You can press ENTER or close the prompt to finish this process"
else
  echo "**ERR**"
  echo "Please check your environment variables or try again your password"
  echo "**ERR**"
fi
read

