			/!\ MANDATORY /!\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
| 	If you are working on airbus desktop, skip this step 		|
|									|
|	You need to create an envrionement variable named AIRBUSID with |
|			your airbus id.					|
|									|
|				exemple 				|
|			AIRBUSID = MyId123				|
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


Resources folder contains two things :
	One folder **buildDependencies**, it contains 2 things:
	 -> AppConfig.ini (Modify it to change your environment)
	 -> Folder **AlwaysCopyInRoot** (Contains all dependencies copied when you build project)
	
	One script setupEnv.sh :
	  -> This script allow you to download a standard envrionement from artifactory